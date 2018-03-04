#include "pch.h"
#include "../early_go/src/inline_macro.hpp"

TEST(inline_macro_test, safe_delete_test1)
{
  int* p{};
  early_go::safe_delete(p);
  EXPECT_EQ(p, nullptr);
}

TEST(inline_macro_test, safe_delete_test2)
{
  int* p{new int{42}};
  early_go::safe_delete(p);
  EXPECT_EQ(p, nullptr);
}

TEST(inline_macro_test, safe_delete_test3)
{
  int* p{new int{42}};
  delete p;
  p = nullptr;
  early_go::safe_delete(p);
  EXPECT_EQ(p, nullptr);
}

TEST(inline_macro_test, safe_delete_test4)
{
  bool _is_called{false};
  struct stub
  {
    bool* p_is_called_{};
    stub(bool* a_is_called) : p_is_called_{a_is_called} {}
    ~stub()
    {
      *p_is_called_ = true;
    }
  };
  stub* s{new stub{&_is_called}};
  early_go::safe_delete(s);
  EXPECT_EQ(_is_called, true);
}

TEST(inline_macro_test, safe_delete_array_test1)
{
  int *pa{};
  early_go::safe_delete_array(pa);
  EXPECT_EQ(pa, nullptr);
}

TEST(inline_macro_test, safe_delete_array_test2)
{
  int *pa = new int[42]{};
  early_go::safe_delete_array(pa);
  EXPECT_EQ(pa, nullptr);
}

TEST(inline_macro_test, safe_delete_array_test3)
{
  int *pa = new int[42]{};
  delete[] pa;
  pa = nullptr;
  early_go::safe_delete_array(pa);
  EXPECT_EQ(pa, nullptr);
}

TEST(inline_macro_test, safe_delete_array_test4)
{
  bool _is_called[42]{false};
  struct stub
  {
    bool* p_is_called_{};
    ~stub()
    {
      *p_is_called_ = true;
    }
  };
  stub* s{new stub[42]};
  for (int i{}; i < 42; ++i) {
    s[i].p_is_called_ = &_is_called[i];
  }
  early_go::safe_delete_array(s);
  for (int i{}; i < 42; ++i) {
    EXPECT_EQ(_is_called[i], true);
  }
}

TEST(inline_macro_test, safe_release_test1)
{
  bool _is_called{false};
  struct stub_has_release
  {
    bool* p_is_called_{};
    stub_has_release(bool* a_is_called) : p_is_called_{a_is_called} {}
    void Release()
    {
      *p_is_called_ = true;
    }
  };
  stub_has_release* p_stub{new stub_has_release{&_is_called}};
  early_go::safe_release(p_stub);
  EXPECT_EQ(_is_called, true);
}

TEST(inline_macro_test, safe_release_test2)
{
  bool _is_called{false};
  struct stub_has_release
  {
    bool* p_is_called_{};
    stub_has_release(bool* a_is_called) : p_is_called_{a_is_called} {}
    void Release()
    {
      *p_is_called_ = true;
    }
  };
  stub_has_release* p_stub{new stub_has_release{&_is_called}};
  p_stub->Release();
  p_stub = nullptr;
  early_go::safe_release(p_stub);
  EXPECT_EQ(p_stub, nullptr);
}

TEST(inline_macro_test, custom_deleter_test1)
{
  bool _is_called{false};
  struct stub_has_release
  {
    bool* p_is_called_{};
    stub_has_release(bool* a_is_called) : p_is_called_{a_is_called} {}
    void Release()
    {
      *p_is_called_ = true;
    }
  };
  {
    std::unique_ptr<stub_has_release, early_go::custom_deleter> up_stub{
        new stub_has_release{&_is_called}, early_go::custom_deleter{}};
  }
  EXPECT_EQ(_is_called, true);
}

TEST(inline_macro_test, custom_deleter_test2)
{
  int _is_called{};
  struct stub_has_release
  {
    int* p_is_called_{};
    stub_has_release(int* a_is_called) : p_is_called_{a_is_called} {}
    void Release()
    {
      ++*p_is_called_;
    }
  };
  {
    std::shared_ptr<stub_has_release> sp_stub{
        new stub_has_release{&_is_called}, early_go::custom_deleter{}};
    std::shared_ptr<stub_has_release> sp_stub2{sp_stub};
    std::shared_ptr<stub_has_release> sp_stub3;
    sp_stub3 = sp_stub;
  }
  EXPECT_EQ(_is_called, 1);
}

TEST(inline_macro_test, log_liner_test1)
{
  early_go::log_liner liner;
  liner << "hogehoge";
  EXPECT_EQ(liner.ostringstream_.str(), "hogehoge");
}

TEST(inline_macro_test, log_liner_test2)
{
  early_go::log_liner liner;
  liner << 123;
  EXPECT_EQ(liner.ostringstream_.str(), "123");
}
