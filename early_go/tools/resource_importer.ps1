# constant value
$DLL_PATH = ($pwd.ToString() + "\" + "..\..\sqlite3\Util\System.Data.SQLite.dll")
$DATABASE_PATH = ($pwd.ToString() + "\" + "..\.res")
$RESOURCE_DIRECTORY = "..\res"

# Check DLL path.
if (!(Test-Path $DLL_PATH)) {
    Write-Output "Error: ${DLL_PATH} cannot be found. Exit."
    exit
}

# Check resource directory.
if (!(Test-Path $RESOURCE_DIRECTORY)) {
    Write-Output "Error: `"${RESOURCE_DIRECTORY}`" folder cannot be found. Exit."
    exit
}

# Load System.Data.sqlite.dll.
[System.Reflection.Assembly]::LoadFile($DLL_PATH) > $null

# Connect a database.
$sqlite = New-Object System.Data.SQLite.SQLiteConnection
$sqlite.ConnectionString = "Data Source = ${DATABASE_PATH}"
$sqlcmd = New-Object System.Data.SQLite.SQLiteCommand
$sqlcmd.Connection = $sqlite

# This script creates a database if it does not exist.
$sqlite.Open()

try {
    # Create tables with the same name as the folders under the resource directory.
    $table = Get-ChildItem -Directory $RESOURCE_DIRECTORY

    for ($i = 0; $i -lt $table.Length; $i++) {
        $table_name = ${table}[$i].Name

        # Create table if it does not exist.
        $sql = "CREATE TABLE IF NOT EXISTS ${table_name} (
                    filename  TEXT,
                    data      BLOB NOT NULL,
                    timestamp TEXT,
                    PRIMARY KEY(filename)
                );"

        $sqlcmd.CommandText = $sql
        $sqlcmd.ExecuteNonQuery() > $null
    
        # Delete unnecessary files.
        $sql ="SELECT filename from ${table_name};"
        $sqlcmd.CommandText = $sql
        $result =  $sqlcmd.ExecuteReader()
        $delete_file_list = @()
        while ($result.Read()) {
            $relative_path = $RESOURCE_DIRECTORY + "\" + $result["filename"]
            if (!(Test-Path $relative_path)) {
                $delete_file_list += $result["filename"]
            }
        }
        $result.Close()
        foreach ($delete_file in $delete_file_list) {
            Write-Output "Delete: ${delete_file}"
            $sql = "DELETE FROM ${table_name}
                    WHERE filename = `"${delete_file}`";"
                        
            $sqlcmd.CommandText = $sql
            $sqlcmd.ExecuteNonQuery() > $null
        }
        
        $table_directory = $RESOURCE_DIRECTORY + "\" + $table_name
        $relative_paths = Get-ChildItem -Recurse -File $table_directory | Resolve-Path -Relative
    
        # Upsert files.
        for ($j=0; $j -lt $relative_paths.Length; $j++) {
            # Create "filename" column data.
            $filename = $relative_paths[$j].Substring($RESOURCE_DIRECTORY.Length+1);
            $filename = $filename.Replace("\", "/");
            
            # Get a timestamp of the resource folder.
            $relative_path = Get-ChildItem $relative_paths[$j]
            $resource_timestamp = $relative_path.LastWriteTime
            
            # Check whether there is necessity to insert or update.
            $sql = "SELECT count(*) from ${table_name}
                    WHERE filename  = `"${filename}`"
                    AND   timestamp = `"${resource_timestamp}`";"

            $sqlcmd.CommandText = $sql
            $result =  $sqlcmd.ExecuteScalar()
            
            if ($result -eq 0) {
                Write-Output "Upsert: $filename"
                $sql = "INSERT OR REPLACE INTO ${table_name} VALUES (
                            `"${filename}`",
                            @placeholder,
                            `"${resource_timestamp}`"
                        );"
                        
                $sqlcmd.CommandText = $sql
                $parameter = New-Object System.Data.SQLite.SQLiteParameter
                # TOOOOOOOOOOOOOOOOOOOOOOO SLOW
                # [Byte[]] $buffer = Get-Content $relative_paths[$j] -Encoding Byte
                $full_filepath = Resolve-Path $relative_paths[$j]
                $fstream = New-Object System.IO.FileStream($full_filepath.ToString(),
                        [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read)
                $buffer = New-Object System.Byte[] $fstream.Length
                $fstream.Read($buffer, 0, $buffer.Length) > $null
                $parameter.Value = $buffer
                $parameter.ParameterName = "@placeholder"
                $sqlcmd.Parameters.Add($parameter) > $null
                
                $sqlcmd.ExecuteNonQuery() > $null
            }
        }
    }
} catch {
    Write-Output $_

    $sqlcmd.Dispose()
    $sqlite.Close()
    exit
}

$sqlcmd.Dispose()
$sqlite.Close()

Read-Host "Please press Enter key"
