#include <wheels/test/test_framework.hpp>

#include <fallible/context/make.hpp>

TEST_SUITE(Context) {
  SIMPLE_TEST(Ctx) {
    auto source = wheels::SourceLocation::Current();

    auto ctx = fallible::Ctx()
      .Domain("Test")
      .Reason("Reason")
      .Location(source)
      .Attr("key", "value")
      .Done();

    ASSERT_EQ(ctx.Domain(), "Test");
    ASSERT_EQ(ctx.Reason(), "Reason");

    auto loc = ctx.Source();
    std::cout << loc.Line();
    ASSERT_EQ(loc.Line(), 7);

    auto attrs = ctx.Attrs();
    ASSERT_EQ(attrs.size(), 1);
  }
}
