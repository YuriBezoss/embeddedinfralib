#include "gtest/gtest.h"
#include "infra/util/Optional.hpp"

namespace infra
{
    void OptionalCoverageDummy();
}

TEST(OptionalTest, OptionalCoverageDummy)
{
    infra::OptionalCoverageDummy();
}

TEST(OptionalTest, TestConstructedEmpty)
{
    infra::Optional<bool> o;
    EXPECT_FALSE(o);
    EXPECT_TRUE(!o);
}

TEST(OptionalTest, TestConstructedEmptyWithNone)
{
    infra::Optional<bool> o(infra::none);
    EXPECT_FALSE(o);
}

TEST(OptionalTest, TestConstructedWithValue)
{
    bool value(true);
    infra::Optional<bool> o(infra::inPlace, value);
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
    EXPECT_TRUE(*(o.operator->()));
}

TEST(OptionalTest, TestConstructedWithMovedValue)
{
    bool value(true);
    infra::Optional<bool> o(infra::inPlace, std::move(value));
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalTest, TestCopyConstruction)
{
    infra::Optional<bool> o1(infra::inPlace, true);
    infra::Optional<bool> o2(o1);
    EXPECT_TRUE(static_cast<bool>(o1));
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_TRUE(*o2);
}

TEST(OptionalTest, TestMoveConstruction)
{
    infra::Optional<bool> o1(infra::inPlace, true);
    infra::Optional<bool> o2(std::move(o1));
    EXPECT_FALSE(o1);
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_TRUE(*o2);
}

TEST(OptionalTest, TestMoveConstructedWithEmptyOptional)
{
    infra::Optional<bool> e;
    infra::Optional<bool> o(std::move(e));
    EXPECT_FALSE(static_cast<bool>(o));
}

TEST(OptionalTest, TestCopyAssign)
{
    infra::Optional<bool> o1(infra::inPlace, true);
    infra::Optional<bool> o2;
    o2 = o1;
    EXPECT_TRUE(static_cast<bool>(o1));
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_TRUE(*o2);
}

TEST(OptionalTest, TestCopyAssignWithSelf)
{
    infra::Optional<bool> o1(infra::inPlace, true);
    o1 = o1;
    EXPECT_TRUE(static_cast<bool>(o1));
    EXPECT_TRUE(*o1);
}

TEST(OptionalTest, TestCopyAssignWithEmptyOptional)
{
    infra::Optional<bool> o1;
    infra::Optional<bool> o2;
    o2 = o1;
    EXPECT_FALSE(static_cast<bool>(o2));
}

TEST(OptionalTest, TestMoveAssign)
{
    infra::Optional<bool> o1(infra::inPlace, true);
    infra::Optional<bool> o2;
    o2 = std::move(o1);
    EXPECT_FALSE(static_cast<bool>(o1));
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_TRUE(*o2);
}

TEST(OptionalTest, TestAssignValue)
{
    infra::Optional<bool> o;
    o = true;
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalTest, TestAssignNone)
{
    infra::Optional<bool> o(infra::inPlace, true);
    o = infra::none;
    EXPECT_FALSE(o);
}

TEST(OptionalTest, TestCompareToNone)
{
    infra::Optional<bool> o;
    EXPECT_TRUE(o == infra::none);
    EXPECT_FALSE(o != infra::none);
    EXPECT_TRUE(infra::none == o);
    EXPECT_FALSE(infra::none != o);
}

TEST(OptionalTest, TestCompare)
{
    infra::Optional<bool> t(infra::inPlace, true);
    infra::Optional<bool> f(infra::inPlace, false);
    infra::Optional<bool> u;

    EXPECT_TRUE(t == t);
    EXPECT_TRUE(f == f);
    EXPECT_TRUE(u == u);

    EXPECT_FALSE(t == f);
    EXPECT_FALSE(t == u);

    EXPECT_FALSE(f == u);

    EXPECT_FALSE(t != t);
}

TEST(OptionalTest, TestCompareToValue)
{
    infra::Optional<bool> t(infra::inPlace, true);
    infra::Optional<bool> u;

    EXPECT_TRUE(t == true);
    EXPECT_TRUE(true == t);
    EXPECT_FALSE(t == false);

    EXPECT_FALSE(t != true);
    EXPECT_FALSE(true != t);

    EXPECT_FALSE(u == true);
}

TEST(OptionalTest, TestConstruct)
{
    bool b(true);
    infra::Optional<bool> o;
    o.Emplace(std::move(b));
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalTest, TestConstructWithInitializerList)
{
    struct X
    {
        X(std::initializer_list<bool>)
        {}
    };
    infra::Optional<X> o;
    o.Emplace({ true });
    EXPECT_TRUE(static_cast<bool>(o));
}

TEST(OptionalTest, TestIndirect)
{
    struct X
    {
        X(): x(true) {}
        bool x;
    };

    infra::Optional<X> o(infra::inPlace, X());
    EXPECT_TRUE(o->x);
}

TEST(OptionalTest, TestMakeOptional)
{
    infra::Optional<bool> o = infra::MakeOptional(true);
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalTest, ValueOrGivesStoredWhenAvailable)
{
    infra::Optional<int> i(infra::inPlace, 5);
    EXPECT_EQ(5, i.ValueOr(2));
}

TEST(OptionalTest, ValueOrGivesParameterWhenNothingIsStored)
{
    infra::Optional<int> i;
    EXPECT_EQ(2, i.ValueOr(2));
}

struct PolymorphicBool
{
    PolymorphicBool(bool value)
        : value(value)
    {}

    virtual ~PolymorphicBool() = default;

    operator bool() const
    {
        return value;
    }

    bool value;
};

TEST(OptionalForPolymorphicObjectsTest, TestConstructedEmpty)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o;
    EXPECT_FALSE(o);
    EXPECT_TRUE(!o);
}

TEST(OptionalForPolymorphicObjectsTest, TestConstructedEmptyWithNone)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o(infra::none);
    EXPECT_FALSE(static_cast<bool>(o));
}

TEST(OptionalForPolymorphicObjectsTest, TestConstructedWithValue)
{
    bool value(true);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o(infra::InPlaceType<PolymorphicBool>(), value);
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalForPolymorphicObjectsTest, TestConstructedWithMovedValue)
{
    bool value(true);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o(infra::InPlaceType<PolymorphicBool>(), std::move(value));
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalForPolymorphicObjectsTest, TestAssignValue)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o;
    o = true;
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalForPolymorphicObjectsTest, TestAssignNone)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o(infra::InPlaceType<PolymorphicBool>(), true);
    o = infra::none;
    EXPECT_FALSE(static_cast<bool>(o));
}

TEST(OptionalForPolymorphicObjectsTest, TestCompareToNone)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o;
    EXPECT_TRUE(o == infra::none);
    EXPECT_FALSE(o != infra::none);
    EXPECT_TRUE(infra::none == o);
    EXPECT_FALSE(infra::none != o);
}

TEST(OptionalForPolymorphicObjectsTest, TestCompare)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> t(infra::InPlaceType<PolymorphicBool>(), true);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> f(infra::InPlaceType<PolymorphicBool>(), false);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> u;

    EXPECT_TRUE(t == t);
    EXPECT_TRUE(f == f);
    EXPECT_TRUE(u == u);

    EXPECT_FALSE(t == f);
    EXPECT_FALSE(t == u);

    EXPECT_FALSE(f == u);
}

TEST(OptionalForPolymorphicObjectsTest, TestCompareToValue)
{
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> t(infra::InPlaceType<PolymorphicBool>(), true);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> u;

    EXPECT_TRUE(t == true);
    EXPECT_TRUE(true == t);
    EXPECT_FALSE(t == false);

    EXPECT_FALSE(t != true);
    EXPECT_FALSE(true != t);

    EXPECT_FALSE(u == true);
}

TEST(OptionalForPolymorphicObjectsTest, TestConstruct)
{
    PolymorphicBool b(true);
    infra::OptionalForPolymorphicObjects<PolymorphicBool, 4> o;
    o.Emplace<PolymorphicBool>(std::move(b));
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_TRUE(*o);
}

TEST(OptionalForPolymorphicObjectsTest, TestIndirect)
{
    struct X
    {
        X()
            : x(true)
        {}

        virtual ~X() = default;

        bool x;
    };

    infra::OptionalForPolymorphicObjects<X, 4> o{ infra::InPlaceType<X>(), X() };
    EXPECT_TRUE(o->x);
}

TEST(OptionalForPolymorphicObjectsTest, ConstructDescendant)
{
    struct Base
    {
        virtual ~Base() = default;
    };

    struct Derived
        : Base
    {
        int ExtraStorage;
    };

    infra::OptionalForPolymorphicObjects<Base, 4> optionalBase((infra::InPlaceType<Derived>()));
    optionalBase.Emplace<Derived>();
}
