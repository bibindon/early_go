xof 0303txt 0032

template AnimTicksPerSecond {
  <9E415A43-7BA6-4a73-8743-B73D47E88476>
  DWORD AnimTicksPerSecond;
}

template XSkinMeshHeader {
  <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
  WORD nMaxSkinWeightsPerVertex;
  WORD nMaxSkinWeightsPerFace;
  WORD nBones;
}

template SkinWeights {
  <6f0d123b-bad2-4167-a0d0-80224f25fabb>
  STRING transformNodeName;
  DWORD nWeights;
  array DWORD vertexIndices[nWeights];
  array float weights[nWeights];
  Matrix4x4 matrixOffset;
}

Frame Root {
  FrameTransformMatrix {
     1.000000, 0.000000, 0.000000, 0.000000,
     0.000000,-0.000000, 1.000000, 0.000000,
     0.000000, 1.000000, 0.000000, 0.000000,
     0.000000, 0.000000, 0.000000, 1.000000;;
  }
  Frame Cube_004 {
    FrameTransformMatrix {
       0.200000, 0.000000, 0.000000, 0.000000,
       0.000000, 2.666254,-6.322052, 0.000000,
      -0.000000, 0.921409, 0.388594, 0.000000,
       0.264085, 0.087266, 0.431951, 1.000000;;
    }
    Mesh { // Cube_004 mesh
      168;
      -0.250000;-0.055535; 0.094600;,
      -0.250000;-0.048247;-0.105400;,
      -0.250000;-0.055535;-0.105400;,
      -0.250000;-0.048247;-0.105400;,
      -0.050000;-0.048247;-0.037507;,
       0.050000;-0.048247;-0.037507;,
       0.250000;-0.048247; 0.094600;,
       0.250000;-0.055535;-0.105400;,
       0.250000;-0.048247;-0.105400;,
       0.250000;-0.055535;-0.105400;,
       0.125000;-0.055535;-0.023511;,
      -0.250000;-0.055535;-0.105400;,
       0.250000;-0.048247;-0.105400;,
      -0.250000;-0.055535;-0.105400;,
      -0.250000;-0.048247;-0.105400;,
      -0.250000;-0.048247; 0.094600;,
       0.250000;-0.055535; 0.094600;,
       0.250000;-0.048247; 0.094600;,
       0.125000;-0.084075; 0.026489;,
      -0.125000;-0.084075;-0.023511;,
       0.125000;-0.084075;-0.023511;,
       0.125000;-0.055535; 0.026489;,
       0.125000;-0.084075;-0.023511;,
       0.125000;-0.055535;-0.023511;,
      -0.125000;-0.055535;-0.023511;,
      -0.125000;-0.084075; 0.026489;,
      -0.125000;-0.055535; 0.026489;,
       0.125000;-0.055535;-0.023511;,
      -0.125000;-0.084075;-0.023511;,
      -0.125000;-0.055535;-0.023511;,
      -0.125000;-0.055535; 0.026489;,
       0.125000;-0.084075; 0.026489;,
       0.125000;-0.055535; 0.026489;,
      -0.125000;-0.055535; 0.026489;,
       0.250000;-0.055535; 0.094600;,
      -0.250000;-0.055535; 0.094600;,
      -0.050000; 0.032002; 0.049929;,
       0.050000; 0.050000; 0.000000;,
      -0.050000; 0.050000; 0.000000;,
       0.050000; 0.024730; 0.000000;,
       0.050000;-0.048247; 0.037507;,
       0.050000;-0.048247; 0.000000;,
       0.050000; 0.032002;-0.049929;,
      -0.050000;-0.048247;-0.037507;,
      -0.050000; 0.032002;-0.049929;,
      -0.050000; 0.032002; 0.049929;,
       0.050000;-0.048247; 0.037507;,
       0.050000; 0.032002; 0.049929;,
       0.050000; 0.024730; 0.000000;,
       0.050000;-0.048247;-0.037507;,
       0.050000; 0.032002;-0.049929;,
      -0.050000; 0.050000; 0.000000;,
       0.050000; 0.032002;-0.049929;,
      -0.050000; 0.032002;-0.049929;,
      -0.050000; 0.024730; 0.000000;,
      -0.050000; 0.050000; 0.000000;,
      -0.050000; 0.032002;-0.049929;,
       0.050000; 0.050000; 0.000000;,
       0.050000; 0.024730; 0.000000;,
       0.050000; 0.032002;-0.049929;,
      -0.250000;-0.048247; 0.094600;,
       0.250000;-0.048247; 0.094600;,
       0.050000;-0.048247; 0.037507;,
      -0.050000;-0.048247;-0.037507;,
      -0.050000; 0.024730; 0.000000;,
      -0.050000; 0.032002;-0.049929;,
      -0.050000;-0.048247; 0.037507;,
      -0.050000; 0.024730; 0.000000;,
      -0.050000;-0.048247; 0.000000;,
      -0.250000;-0.055535; 0.094600;,
      -0.250000;-0.048247; 0.094600;,
      -0.250000;-0.048247;-0.105400;,
      -0.250000;-0.048247;-0.105400;,
      -0.250000;-0.048247; 0.094600;,
      -0.050000;-0.048247; 0.000000;,
      -0.250000;-0.048247; 0.094600;,
      -0.050000;-0.048247; 0.037507;,
      -0.050000;-0.048247; 0.000000;,
      -0.050000;-0.048247; 0.000000;,
      -0.050000;-0.048247;-0.037507;,
      -0.250000;-0.048247;-0.105400;,
       0.050000;-0.048247;-0.037507;,
       0.250000;-0.048247;-0.105400;,
      -0.250000;-0.048247;-0.105400;,
       0.250000;-0.048247; 0.094600;,
       0.250000;-0.055535; 0.094600;,
       0.250000;-0.055535;-0.105400;,
       0.250000;-0.055535;-0.105400;,
       0.250000;-0.055535; 0.094600;,
       0.125000;-0.055535;-0.023511;,
       0.250000;-0.055535; 0.094600;,
       0.125000;-0.055535; 0.026489;,
       0.125000;-0.055535;-0.023511;,
       0.125000;-0.055535;-0.023511;,
      -0.125000;-0.055535;-0.023511;,
      -0.250000;-0.055535;-0.105400;,
       0.250000;-0.048247;-0.105400;,
       0.250000;-0.055535;-0.105400;,
      -0.250000;-0.055535;-0.105400;,
      -0.250000;-0.048247; 0.094600;,
      -0.250000;-0.055535; 0.094600;,
       0.250000;-0.055535; 0.094600;,
       0.125000;-0.084075; 0.026489;,
      -0.125000;-0.084075; 0.026489;,
      -0.125000;-0.084075;-0.023511;,
       0.125000;-0.055535; 0.026489;,
       0.125000;-0.084075; 0.026489;,
       0.125000;-0.084075;-0.023511;,
      -0.125000;-0.055535;-0.023511;,
      -0.125000;-0.084075;-0.023511;,
      -0.125000;-0.084075; 0.026489;,
       0.125000;-0.055535;-0.023511;,
       0.125000;-0.084075;-0.023511;,
      -0.125000;-0.084075;-0.023511;,
      -0.125000;-0.055535; 0.026489;,
      -0.125000;-0.084075; 0.026489;,
       0.125000;-0.084075; 0.026489;,
      -0.250000;-0.055535; 0.094600;,
      -0.250000;-0.055535;-0.105400;,
      -0.125000;-0.055535;-0.023511;,
      -0.125000;-0.055535; 0.026489;,
       0.125000;-0.055535; 0.026489;,
       0.250000;-0.055535; 0.094600;,
      -0.250000;-0.055535; 0.094600;,
      -0.125000;-0.055535;-0.023511;,
      -0.125000;-0.055535; 0.026489;,
      -0.050000; 0.032002; 0.049929;,
       0.050000; 0.032002; 0.049929;,
       0.050000; 0.050000; 0.000000;,
       0.050000; 0.024730; 0.000000;,
       0.050000; 0.032002; 0.049929;,
       0.050000;-0.048247; 0.037507;,
       0.050000; 0.032002;-0.049929;,
       0.050000;-0.048247;-0.037507;,
      -0.050000;-0.048247;-0.037507;,
      -0.050000; 0.032002; 0.049929;,
      -0.050000;-0.048247; 0.037507;,
       0.050000;-0.048247; 0.037507;,
       0.050000; 0.024730; 0.000000;,
       0.050000;-0.048247; 0.000000;,
       0.050000;-0.048247;-0.037507;,
      -0.050000; 0.050000; 0.000000;,
       0.050000; 0.050000; 0.000000;,
       0.050000; 0.032002;-0.049929;,
      -0.050000; 0.024730; 0.000000;,
      -0.050000; 0.032002; 0.049929;,
      -0.050000; 0.050000; 0.000000;,
       0.050000; 0.050000; 0.000000;,
       0.050000; 0.032002; 0.049929;,
       0.050000; 0.024730; 0.000000;,
       0.250000;-0.048247; 0.094600;,
       0.250000;-0.048247;-0.105400;,
       0.050000;-0.048247; 0.000000;,
       0.250000;-0.048247;-0.105400;,
       0.050000;-0.048247;-0.037507;,
       0.050000;-0.048247; 0.000000;,
       0.050000;-0.048247; 0.000000;,
       0.050000;-0.048247; 0.037507;,
       0.250000;-0.048247; 0.094600;,
      -0.050000;-0.048247; 0.037507;,
      -0.250000;-0.048247; 0.094600;,
       0.050000;-0.048247; 0.037507;,
      -0.050000;-0.048247;-0.037507;,
      -0.050000;-0.048247; 0.000000;,
      -0.050000; 0.024730; 0.000000;,
      -0.050000;-0.048247; 0.037507;,
      -0.050000; 0.032002; 0.049929;,
      -0.050000; 0.024730; 0.000000;;
      56;
      3;2,1,0;,
      3;5,4,3;,
      3;8,7,6;,
      3;11,10,9;,
      3;14,13,12;,
      3;17,16,15;,
      3;20,19,18;,
      3;23,22,21;,
      3;26,25,24;,
      3;29,28,27;,
      3;32,31,30;,
      3;35,34,33;,
      3;38,37,36;,
      3;41,40,39;,
      3;44,43,42;,
      3;47,46,45;,
      3;50,49,48;,
      3;53,52,51;,
      3;56,55,54;,
      3;59,58,57;,
      3;62,61,60;,
      3;65,64,63;,
      3;68,67,66;,
      3;71,70,69;,
      3;74,73,72;,
      3;77,76,75;,
      3;80,79,78;,
      3;83,82,81;,
      3;86,85,84;,
      3;89,88,87;,
      3;92,91,90;,
      3;95,94,93;,
      3;98,97,96;,
      3;101,100,99;,
      3;104,103,102;,
      3;107,106,105;,
      3;110,109,108;,
      3;113,112,111;,
      3;116,115,114;,
      3;119,118,117;,
      3;122,121,120;,
      3;125,124,123;,
      3;128,127,126;,
      3;131,130,129;,
      3;134,133,132;,
      3;137,136,135;,
      3;140,139,138;,
      3;143,142,141;,
      3;146,145,144;,
      3;149,148,147;,
      3;152,151,150;,
      3;155,154,153;,
      3;158,157,156;,
      3;161,160,159;,
      3;164,163,162;,
      3;167,166,165;;
      MeshNormals { // Cube_004 normals
        56;
        -1.000000; 0.000000; 0.000000;,
         0.000000; 1.000000; 0.000000;,
         1.000000; 0.000000;-0.000000;,
         0.000000;-1.000000; 0.000000;,
         0.000000; 0.000000;-1.000000;,
         0.000000; 0.000000; 1.000000;,
         0.000000;-1.000000; 0.000000;,
         1.000000; 0.000000;-0.000000;,
        -1.000000;-0.000000;-0.000000;,
         0.000000; 0.000000;-1.000000;,
         0.000000; 0.000000; 1.000000;,
         0.000000;-1.000000;-0.000000;,
        -0.000000; 0.940746; 0.339112;,
         1.000000;-0.000000; 0.000000;,
         0.000000;-0.152965;-0.988232;,
         0.000000;-0.152965; 0.988232;,
         1.000000;-0.000000; 0.000000;,
         0.000000; 0.940746;-0.339112;,
        -1.000000;-0.000000; 0.000000;,
         1.000000;-0.000000; 0.000000;,
         0.000000; 1.000000;-0.000000;,
        -1.000000;-0.000000; 0.000000;,
        -1.000000; 0.000000; 0.000000;,
        -1.000000; 0.000000; 0.000000;,
         0.000000; 1.000000; 0.000000;,
         0.000000; 1.000000;-0.000000;,
         0.000000; 1.000000; 0.000000;,
         0.000000; 1.000000;-0.000000;,
         1.000000; 0.000000; 0.000000;,
         0.000000;-1.000000; 0.000000;,
         0.000000;-1.000000; 0.000000;,
         0.000000;-1.000000; 0.000000;,
         0.000000; 0.000000;-1.000000;,
         0.000000;-0.000000; 1.000000;,
         0.000000;-1.000000; 0.000000;,
         1.000000; 0.000000; 0.000000;,
        -1.000000; 0.000000; 0.000000;,
         0.000000; 0.000000;-1.000000;,
         0.000000;-0.000000; 1.000000;,
        -0.000000;-1.000000; 0.000000;,
        -0.000000;-1.000000; 0.000000;,
        -0.000000;-1.000000;-0.000000;,
         0.000000; 0.940746; 0.339112;,
         1.000000;-0.000000;-0.000000;,
         0.000000;-0.152965;-0.988232;,
         0.000000;-0.152965; 0.988232;,
         1.000000; 0.000000; 0.000000;,
         0.000000; 0.940746;-0.339112;,
        -1.000000;-0.000000;-0.000000;,
         1.000000;-0.000000;-0.000000;,
        -0.000000; 1.000000; 0.000000;,
         0.000000; 1.000000; 0.000000;,
         0.000000; 1.000000; 0.000000;,
         0.000000; 1.000000; 0.000000;,
        -1.000000;-0.000000;-0.000000;,
        -1.000000;-0.000000;-0.000000;;
        56;
        3;0,0,0;,
        3;1,1,1;,
        3;2,2,2;,
        3;3,3,3;,
        3;4,4,4;,
        3;5,5,5;,
        3;6,6,6;,
        3;7,7,7;,
        3;8,8,8;,
        3;9,9,9;,
        3;10,10,10;,
        3;11,11,11;,
        3;12,12,12;,
        3;13,13,13;,
        3;14,14,14;,
        3;15,15,15;,
        3;16,16,16;,
        3;17,17,17;,
        3;18,18,18;,
        3;19,19,19;,
        3;20,20,20;,
        3;21,21,21;,
        3;22,22,22;,
        3;23,23,23;,
        3;24,24,24;,
        3;25,25,25;,
        3;26,26,26;,
        3;27,27,27;,
        3;28,28,28;,
        3;29,29,29;,
        3;30,30,30;,
        3;31,31,31;,
        3;32,32,32;,
        3;33,33,33;,
        3;34,34,34;,
        3;35,35,35;,
        3;36,36,36;,
        3;37,37,37;,
        3;38,38,38;,
        3;39,39,39;,
        3;40,40,40;,
        3;41,41,41;,
        3;42,42,42;,
        3;43,43,43;,
        3;44,44,44;,
        3;45,45,45;,
        3;46,46,46;,
        3;47,47,47;,
        3;48,48,48;,
        3;49,49,49;,
        3;50,50,50;,
        3;51,51,51;,
        3;52,52,52;,
        3;53,53,53;,
        3;54,54,54;,
        3;55,55,55;;
      } // End of Cube_004 normals
      MeshTextureCoords { // Cube_004 UV coordinates
        168;
         0.975446; 0.987342;,
         0.985796; 0.734177;,
         0.975446; 0.734177;,
         0.625527; 0.712048;,
         0.529094; 0.823914;,
         0.511897; 0.879847;,
         0.966146; 0.392405;,
         0.955795; 0.645570;,
         0.966146; 0.645570;,
         0.028968; 0.991712;,
         0.145282; 0.921796;,
         0.028969; 0.712048;,
         0.848158; 0.645570;,
         0.858495; 0.012658;,
         0.848158; 0.012658;,
         0.819750; 0.012658;,
         0.809413; 0.645570;,
         0.819750; 0.645570;,
         0.710189; 0.987342;,
         0.639170; 0.670886;,
         0.639170; 0.987342;,
         0.028968; 0.700862;,
         0.069507; 0.672896;,
         0.028968; 0.672896;,
         0.097914; 0.700862;,
         0.138453; 0.672896;,
         0.097914; 0.672896;,
         0.906552; 0.987342;,
         0.947038; 0.670886;,
         0.906553; 0.670886;,
         0.927388; 0.329114;,
         0.886902; 0.645570;,
         0.927388; 0.645570;,
         0.216301; 0.781964;,
         0.313044; 0.991712;,
         0.313044; 0.712048;,
         0.781005; 0.645570;,
         0.710088; 0.518987;,
         0.710088; 0.645570;,
         0.155355; 0.499902;,
         0.575205; 0.577091;,
         0.575205; 0.499902;,
         0.639170; 0.341953;,
         0.753908; 0.215370;,
         0.639170; 0.215370;,
         0.853334; 0.709041;,
         0.738596; 0.835624;,
         0.853334; 0.835624;,
         0.155355; 0.499902;,
         0.575205; 0.422713;,
         0.113520; 0.397150;,
         0.710088; 0.645570;,
         0.639170; 0.518987;,
         0.639170; 0.645570;,
         0.155287; 0.215890;,
         0.004634; 0.215890;,
         0.111934; 0.325990;,
         0.009976; 0.499902;,
         0.155355; 0.499902;,
         0.113520; 0.397150;,
         0.341452; 0.712048;,
         0.341452; 0.991712;,
         0.422545; 0.879847;,
         0.590368; 0.298599;,
         0.155287; 0.215890;,
         0.111934; 0.325990;,
         0.590368; 0.133181;,
         0.155287; 0.215890;,
         0.590368; 0.215890;,
         0.975446; 0.987342;,
         0.985796; 0.987342;,
         0.985796; 0.734177;,
         0.625527; 0.712048;,
         0.341452; 0.712048;,
         0.475820; 0.823914;,
         0.341452; 0.712048;,
         0.422545; 0.823914;,
         0.475820; 0.823914;,
         0.475820; 0.823914;,
         0.529094; 0.823914;,
         0.625527; 0.712048;,
         0.511897; 0.879847;,
         0.625527; 0.991712;,
         0.625527; 0.712048;,
         0.966146; 0.392405;,
         0.955795; 0.392405;,
         0.955795; 0.645570;,
         0.028968; 0.991712;,
         0.313044; 0.991712;,
         0.145282; 0.921796;,
         0.313044; 0.991712;,
         0.216301; 0.921796;,
         0.145282; 0.921796;,
         0.145282; 0.921796;,
         0.145282; 0.781964;,
         0.028969; 0.712048;,
         0.848158; 0.645570;,
         0.858495; 0.645570;,
         0.858495; 0.012658;,
         0.819750; 0.012658;,
         0.809413; 0.012658;,
         0.809413; 0.645570;,
         0.710189; 0.987342;,
         0.710189; 0.670886;,
         0.639170; 0.670886;,
         0.028968; 0.700862;,
         0.069507; 0.700862;,
         0.069507; 0.672896;,
         0.097914; 0.700862;,
         0.138453; 0.700862;,
         0.138453; 0.672896;,
         0.906552; 0.987342;,
         0.947038; 0.987342;,
         0.947038; 0.670886;,
         0.927388; 0.329114;,
         0.886902; 0.329114;,
         0.886902; 0.645570;,
         0.313044; 0.712048;,
         0.028969; 0.712048;,
         0.145282; 0.781964;,
         0.216301; 0.781964;,
         0.216301; 0.921796;,
         0.313044; 0.991712;,
         0.313044; 0.712048;,
         0.145282; 0.781964;,
         0.216301; 0.781964;,
         0.781005; 0.645570;,
         0.781005; 0.518987;,
         0.710088; 0.518987;,
         0.155355; 0.499902;,
         0.113520; 0.602654;,
         0.575205; 0.577091;,
         0.639170; 0.341953;,
         0.753908; 0.341953;,
         0.753908; 0.215370;,
         0.853334; 0.709041;,
         0.738597; 0.709041;,
         0.738596; 0.835624;,
         0.155355; 0.499902;,
         0.575205; 0.499902;,
         0.575205; 0.422713;,
         0.710088; 0.645570;,
         0.710088; 0.518987;,
         0.639170; 0.518987;,
         0.155287; 0.215890;,
         0.111934; 0.105789;,
         0.004634; 0.215890;,
         0.009976; 0.499902;,
         0.113520; 0.602654;,
         0.155355; 0.499902;,
         0.341452; 0.991712;,
         0.625527; 0.991712;,
         0.475820; 0.879847;,
         0.625527; 0.991712;,
         0.511897; 0.879847;,
         0.475820; 0.879847;,
         0.475820; 0.879847;,
         0.422545; 0.879847;,
         0.341452; 0.991712;,
         0.422545; 0.823914;,
         0.341452; 0.712048;,
         0.422545; 0.879847;,
         0.590368; 0.298599;,
         0.590368; 0.215890;,
         0.155287; 0.215890;,
         0.590368; 0.133181;,
         0.111934; 0.105789;,
         0.155287; 0.215890;;
      } // End of Cube_004 UV coordinates
      MeshMaterialList { // Cube_004 material list
        1;
        56;
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0;;
        Material saber {
           0.640000; 0.640000; 0.640000; 1.000000;;
           96.078431;
           0.500000; 0.500000; 0.500000;;
           0.000000; 0.000000; 0.000000;;
          TextureFilename {"saber.png";}
        }
      } // End of Cube_004 material list
    } // End of Cube_004 mesh
  } // End of Cube_004
} // End of Root
AnimTicksPerSecond {
  24;
}
AnimationSet Default_Action {
  Animation {
    {Cube_004}
    AnimationKey { // Rotation
      0;
      100;
      0;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      1;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      2;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      3;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      4;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      5;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      6;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      7;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      8;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      9;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      10;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      11;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      12;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      13;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      14;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      15;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      16;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      17;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      18;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      19;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      20;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      21;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      22;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      23;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      24;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      25;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      26;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      27;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      28;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      29;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      30;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      31;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      32;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      33;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      34;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      35;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      36;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      37;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      38;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      39;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      40;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      41;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      42;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      43;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      44;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      45;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      46;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      47;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      48;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      49;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      50;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      51;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      52;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      53;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      54;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      55;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      56;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      57;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      58;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      59;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      60;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      61;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      62;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      63;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      64;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      65;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      66;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      67;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      68;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      69;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      70;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      71;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      72;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      73;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      74;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      75;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      76;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      77;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      78;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      79;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      80;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      81;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      82;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      83;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      84;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      85;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      86;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      87;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      88;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      89;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      90;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      91;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      92;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      93;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      94;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      95;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      96;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      97;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      98;4;-0.833245,-0.552904,-0.000000, 0.000000;;,
      99;4;-0.833245,-0.552904,-0.000000, 0.000000;;;
    }
    AnimationKey { // Scale
      1;
      100;
      0;3; 0.200000, 6.861287, 1.000000;;,
      1;3; 0.200000, 6.861287, 1.000000;;,
      2;3; 0.200000, 6.861287, 1.000000;;,
      3;3; 0.200000, 6.861287, 1.000000;;,
      4;3; 0.200000, 6.861287, 1.000000;;,
      5;3; 0.200000, 6.861287, 1.000000;;,
      6;3; 0.200000, 6.861287, 1.000000;;,
      7;3; 0.200000, 6.861287, 1.000000;;,
      8;3; 0.200000, 6.861287, 1.000000;;,
      9;3; 0.200000, 6.861287, 1.000000;;,
      10;3; 0.200000, 6.861287, 1.000000;;,
      11;3; 0.200000, 6.861287, 1.000000;;,
      12;3; 0.200000, 6.861287, 1.000000;;,
      13;3; 0.200000, 6.861287, 1.000000;;,
      14;3; 0.200000, 6.861287, 1.000000;;,
      15;3; 0.200000, 6.861287, 1.000000;;,
      16;3; 0.200000, 6.861287, 1.000000;;,
      17;3; 0.200000, 6.861287, 1.000000;;,
      18;3; 0.200000, 6.861287, 1.000000;;,
      19;3; 0.200000, 6.861287, 1.000000;;,
      20;3; 0.200000, 6.861287, 1.000000;;,
      21;3; 0.200000, 6.861287, 1.000000;;,
      22;3; 0.200000, 6.861287, 1.000000;;,
      23;3; 0.200000, 6.861287, 1.000000;;,
      24;3; 0.200000, 6.861287, 1.000000;;,
      25;3; 0.200000, 6.861287, 1.000000;;,
      26;3; 0.200000, 6.861287, 1.000000;;,
      27;3; 0.200000, 6.861287, 1.000000;;,
      28;3; 0.200000, 6.861287, 1.000000;;,
      29;3; 0.200000, 6.861287, 1.000000;;,
      30;3; 0.200000, 6.861287, 1.000000;;,
      31;3; 0.200000, 6.861287, 1.000000;;,
      32;3; 0.200000, 6.861287, 1.000000;;,
      33;3; 0.200000, 6.861287, 1.000000;;,
      34;3; 0.200000, 6.861287, 1.000000;;,
      35;3; 0.200000, 6.861287, 1.000000;;,
      36;3; 0.200000, 6.861287, 1.000000;;,
      37;3; 0.200000, 6.861287, 1.000000;;,
      38;3; 0.200000, 6.861287, 1.000000;;,
      39;3; 0.200000, 6.861287, 1.000000;;,
      40;3; 0.200000, 6.861287, 1.000000;;,
      41;3; 0.200000, 6.861287, 1.000000;;,
      42;3; 0.200000, 6.861287, 1.000000;;,
      43;3; 0.200000, 6.861287, 1.000000;;,
      44;3; 0.200000, 6.861287, 1.000000;;,
      45;3; 0.200000, 6.861287, 1.000000;;,
      46;3; 0.200000, 6.861287, 1.000000;;,
      47;3; 0.200000, 6.861287, 1.000000;;,
      48;3; 0.200000, 6.861287, 1.000000;;,
      49;3; 0.200000, 6.861287, 1.000000;;,
      50;3; 0.200000, 6.861287, 1.000000;;,
      51;3; 0.200000, 6.861287, 1.000000;;,
      52;3; 0.200000, 6.861287, 1.000000;;,
      53;3; 0.200000, 6.861287, 1.000000;;,
      54;3; 0.200000, 6.861287, 1.000000;;,
      55;3; 0.200000, 6.861287, 1.000000;;,
      56;3; 0.200000, 6.861287, 1.000000;;,
      57;3; 0.200000, 6.861287, 1.000000;;,
      58;3; 0.200000, 6.861287, 1.000000;;,
      59;3; 0.200000, 6.861287, 1.000000;;,
      60;3; 0.200000, 6.861287, 1.000000;;,
      61;3; 0.200000, 6.861287, 1.000000;;,
      62;3; 0.200000, 6.861287, 1.000000;;,
      63;3; 0.200000, 6.861287, 1.000000;;,
      64;3; 0.200000, 6.861287, 1.000000;;,
      65;3; 0.200000, 6.861287, 1.000000;;,
      66;3; 0.200000, 6.861287, 1.000000;;,
      67;3; 0.200000, 6.861287, 1.000000;;,
      68;3; 0.200000, 6.861287, 1.000000;;,
      69;3; 0.200000, 6.861287, 1.000000;;,
      70;3; 0.200000, 6.861287, 1.000000;;,
      71;3; 0.200000, 6.861287, 1.000000;;,
      72;3; 0.200000, 6.861287, 1.000000;;,
      73;3; 0.200000, 6.861287, 1.000000;;,
      74;3; 0.200000, 6.861287, 1.000000;;,
      75;3; 0.200000, 6.861287, 1.000000;;,
      76;3; 0.200000, 6.861287, 1.000000;;,
      77;3; 0.200000, 6.861287, 1.000000;;,
      78;3; 0.200000, 6.861287, 1.000000;;,
      79;3; 0.200000, 6.861287, 1.000000;;,
      80;3; 0.200000, 6.861287, 1.000000;;,
      81;3; 0.200000, 6.861287, 1.000000;;,
      82;3; 0.200000, 6.861287, 1.000000;;,
      83;3; 0.200000, 6.861287, 1.000000;;,
      84;3; 0.200000, 6.861287, 1.000000;;,
      85;3; 0.200000, 6.861287, 1.000000;;,
      86;3; 0.200000, 6.861287, 1.000000;;,
      87;3; 0.200000, 6.861287, 1.000000;;,
      88;3; 0.200000, 6.861287, 1.000000;;,
      89;3; 0.200000, 6.861287, 1.000000;;,
      90;3; 0.200000, 6.861287, 1.000000;;,
      91;3; 0.200000, 6.861287, 1.000000;;,
      92;3; 0.200000, 6.861287, 1.000000;;,
      93;3; 0.200000, 6.861287, 1.000000;;,
      94;3; 0.200000, 6.861287, 1.000000;;,
      95;3; 0.200000, 6.861287, 1.000000;;,
      96;3; 0.200000, 6.861287, 1.000000;;,
      97;3; 0.200000, 6.861287, 1.000000;;,
      98;3; 0.200000, 6.861287, 1.000000;;,
      99;3; 0.200000, 6.861287, 1.000000;;;
    }
    AnimationKey { // Position
      2;
      100;
      0;3; 0.264085, 0.087266, 0.431951;;,
      1;3; 0.264085, 0.087266, 0.431951;;,
      2;3; 0.264085, 0.087266, 0.431951;;,
      3;3; 0.264085, 0.087266, 0.431951;;,
      4;3; 0.264085, 0.087266, 0.431951;;,
      5;3; 0.264085, 0.087266, 0.431951;;,
      6;3; 0.264085, 0.087266, 0.431951;;,
      7;3; 0.264085, 0.087266, 0.431951;;,
      8;3; 0.264085, 0.087266, 0.431951;;,
      9;3; 0.264085, 0.087266, 0.431951;;,
      10;3; 0.264085, 0.087266, 0.431951;;,
      11;3; 0.264085, 0.087266, 0.431951;;,
      12;3; 0.264085, 0.087266, 0.431951;;,
      13;3; 0.264085, 0.087266, 0.431951;;,
      14;3; 0.264085, 0.087266, 0.431951;;,
      15;3; 0.264085, 0.087266, 0.431951;;,
      16;3; 0.264085, 0.087266, 0.431951;;,
      17;3; 0.264085, 0.087266, 0.431951;;,
      18;3; 0.264085, 0.087266, 0.431951;;,
      19;3; 0.264085, 0.087266, 0.431951;;,
      20;3; 0.264085, 0.087266, 0.431951;;,
      21;3; 0.264085, 0.087266, 0.431951;;,
      22;3; 0.264085, 0.087266, 0.431951;;,
      23;3; 0.264085, 0.087266, 0.431951;;,
      24;3; 0.264085, 0.087266, 0.431951;;,
      25;3; 0.264085, 0.087266, 0.431951;;,
      26;3; 0.264085, 0.087266, 0.431951;;,
      27;3; 0.264085, 0.087266, 0.431951;;,
      28;3; 0.264085, 0.087266, 0.431951;;,
      29;3; 0.264085, 0.087266, 0.431951;;,
      30;3; 0.264085, 0.087266, 0.431951;;,
      31;3; 0.264085, 0.087266, 0.431951;;,
      32;3; 0.264085, 0.087266, 0.431951;;,
      33;3; 0.264085, 0.087266, 0.431951;;,
      34;3; 0.264085, 0.087266, 0.431951;;,
      35;3; 0.264085, 0.087266, 0.431951;;,
      36;3; 0.264085, 0.087266, 0.431951;;,
      37;3; 0.264085, 0.087266, 0.431951;;,
      38;3; 0.264085, 0.087266, 0.431951;;,
      39;3; 0.264085, 0.087266, 0.431951;;,
      40;3; 0.264085, 0.087266, 0.431951;;,
      41;3; 0.264085, 0.087266, 0.431951;;,
      42;3; 0.264085, 0.087266, 0.431951;;,
      43;3; 0.264085, 0.087266, 0.431951;;,
      44;3; 0.264085, 0.087266, 0.431951;;,
      45;3; 0.264085, 0.087266, 0.431951;;,
      46;3; 0.264085, 0.087266, 0.431951;;,
      47;3; 0.264085, 0.087266, 0.431951;;,
      48;3; 0.264085, 0.087266, 0.431951;;,
      49;3; 0.264085, 0.087266, 0.431951;;,
      50;3; 0.264085, 0.087266, 0.431951;;,
      51;3; 0.264085, 0.087266, 0.431951;;,
      52;3; 0.264085, 0.087266, 0.431951;;,
      53;3; 0.264085, 0.087266, 0.431951;;,
      54;3; 0.264085, 0.087266, 0.431951;;,
      55;3; 0.264085, 0.087266, 0.431951;;,
      56;3; 0.264085, 0.087266, 0.431951;;,
      57;3; 0.264085, 0.087266, 0.431951;;,
      58;3; 0.264085, 0.087266, 0.431951;;,
      59;3; 0.264085, 0.087266, 0.431951;;,
      60;3; 0.264085, 0.087266, 0.431951;;,
      61;3; 0.264085, 0.087266, 0.431951;;,
      62;3; 0.264085, 0.087266, 0.431951;;,
      63;3; 0.264085, 0.087266, 0.431951;;,
      64;3; 0.264085, 0.087266, 0.431951;;,
      65;3; 0.264085, 0.087266, 0.431951;;,
      66;3; 0.264085, 0.087266, 0.431951;;,
      67;3; 0.264085, 0.087266, 0.431951;;,
      68;3; 0.264085, 0.087266, 0.431951;;,
      69;3; 0.264085, 0.087266, 0.431951;;,
      70;3; 0.264085, 0.087266, 0.431951;;,
      71;3; 0.264085, 0.087266, 0.431951;;,
      72;3; 0.264085, 0.087266, 0.431951;;,
      73;3; 0.264085, 0.087266, 0.431951;;,
      74;3; 0.264085, 0.087266, 0.431951;;,
      75;3; 0.264085, 0.087266, 0.431951;;,
      76;3; 0.264085, 0.087266, 0.431951;;,
      77;3; 0.264085, 0.087266, 0.431951;;,
      78;3; 0.264085, 0.087266, 0.431951;;,
      79;3; 0.264085, 0.087266, 0.431951;;,
      80;3; 0.264085, 0.087266, 0.431951;;,
      81;3; 0.264085, 0.087266, 0.431951;;,
      82;3; 0.264085, 0.087266, 0.431951;;,
      83;3; 0.264085, 0.087266, 0.431951;;,
      84;3; 0.264085, 0.087266, 0.431951;;,
      85;3; 0.264085, 0.087266, 0.431951;;,
      86;3; 0.264085, 0.087266, 0.431951;;,
      87;3; 0.264085, 0.087266, 0.431951;;,
      88;3; 0.264085, 0.087266, 0.431951;;,
      89;3; 0.264085, 0.087266, 0.431951;;,
      90;3; 0.264085, 0.087266, 0.431951;;,
      91;3; 0.264085, 0.087266, 0.431951;;,
      92;3; 0.264085, 0.087266, 0.431951;;,
      93;3; 0.264085, 0.087266, 0.431951;;,
      94;3; 0.264085, 0.087266, 0.431951;;,
      95;3; 0.264085, 0.087266, 0.431951;;,
      96;3; 0.264085, 0.087266, 0.431951;;,
      97;3; 0.264085, 0.087266, 0.431951;;,
      98;3; 0.264085, 0.087266, 0.431951;;,
      99;3; 0.264085, 0.087266, 0.431951;;;
    }
  }
} // End of AnimationSet Default_Action