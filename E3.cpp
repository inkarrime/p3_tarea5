#include <cmath>
#include <iostream>
#include <memory>
#include <string>

class Expression {
public:
    virtual double evaluate(double x) = 0;
    virtual std::shared_ptr<Expression> derivative() = 0;
    virtual std::string toString() = 0;
    virtual bool isConstant() const { return false; }
    virtual ~Expression() = default;
};

using ExprPtr = std::shared_ptr<Expression>;

class Constant : public Expression {
private:
    double value;

public:
    explicit Constant(double value) : value(value) {}

    double evaluate(double) override { return value; }
    ExprPtr derivative() override { return std::make_shared<Constant>(0); }
    bool isConstant() const override { return true; }

    std::string toString() override {
        if (value == static_cast<long long>(value)) {
            return std::to_string(static_cast<long long>(value));
        }
        return std::to_string(value);
    }
};

class Variable : public Expression {
public:
    double evaluate(double x) override { return x; }
    ExprPtr derivative() override { return std::make_shared<Constant>(1); }
    std::string toString() override { return "x"; }
};

class BinaryOperation : public Expression {
protected:
    ExprPtr left;
    ExprPtr right;
    virtual std::string symbol() const = 0;

public:
    BinaryOperation(ExprPtr left, ExprPtr right) : left(left), right(right) {}

    std::string toString() override {
        return "(" + left->toString() + " " + symbol() + " " + right->toString() + ")";
    }
};

class UnaryFunction : public Expression {
protected:
    ExprPtr arg;
    virtual std::string name() const = 0;

public:
    explicit UnaryFunction(ExprPtr arg) : arg(arg) {}

    std::string toString() override {
        return name() + "(" + arg->toString() + ")";
    }
};

class Add : public BinaryOperation {
protected:
    std::string symbol() const override { return "+"; }

public:
    Add(ExprPtr left, ExprPtr right) : BinaryOperation(left, right) {}
    double evaluate(double x) override { return left->evaluate(x) + right->evaluate(x); }
    ExprPtr derivative() override;
};

class Multiply : public BinaryOperation {
protected:
    std::string symbol() const override { return "*"; }

public:
    Multiply(ExprPtr left, ExprPtr right) : BinaryOperation(left, right) {}
    double evaluate(double x) override { return left->evaluate(x) * right->evaluate(x); }
    ExprPtr derivative() override;
};

class Power : public BinaryOperation {
protected:
    std::string symbol() const override { return "^"; }

public:
    Power(ExprPtr base, ExprPtr exponent) : BinaryOperation(base, exponent) {}
    double evaluate(double x) override { return std::pow(left->evaluate(x), right->evaluate(x)); }
    ExprPtr derivative() override;
};

class Sin : public UnaryFunction {
protected:
    std::string name() const override { return "sin"; }

public:
    explicit Sin(ExprPtr arg) : UnaryFunction(arg) {}
    double evaluate(double x) override { return std::sin(arg->evaluate(x)); }
    ExprPtr derivative() override;
};

class Cos : public UnaryFunction {
protected:
    std::string name() const override { return "cos"; }

public:
    explicit Cos(ExprPtr arg) : UnaryFunction(arg) {}
    double evaluate(double x) override { return std::cos(arg->evaluate(x)); }
    ExprPtr derivative() override;
};

class Ln : public UnaryFunction {
protected:
    std::string name() const override { return "ln"; }

public:
    explicit Ln(ExprPtr arg) : UnaryFunction(arg) {}
    double evaluate(double x) override { return std::log(arg->evaluate(x)); }
    ExprPtr derivative() override;
};

ExprPtr makeAdd(ExprPtr a, ExprPtr b) {
    if (a->isConstant() && b->isConstant()) return std::make_shared<Constant>(a->evaluate(0) + b->evaluate(0));
    if (a->isConstant() && a->evaluate(0) == 0) return b;
    if (b->isConstant() && b->evaluate(0) == 0) return a;
    return std::make_shared<Add>(a, b);
}

ExprPtr makeMul(ExprPtr a, ExprPtr b) {
    if (a->isConstant() && b->isConstant()) return std::make_shared<Constant>(a->evaluate(0) * b->evaluate(0));
    if ((a->isConstant() && a->evaluate(0) == 0) || (b->isConstant() && b->evaluate(0) == 0)) {
        return std::make_shared<Constant>(0);
    }
    if (a->isConstant() && a->evaluate(0) == 1) return b;
    if (b->isConstant() && b->evaluate(0) == 1) return a;
    return std::make_shared<Multiply>(a, b);
}

ExprPtr makePow(ExprPtr base, ExprPtr exponent) {
    if (exponent->isConstant() && exponent->evaluate(0) == 0) return std::make_shared<Constant>(1);
    if (exponent->isConstant() && exponent->evaluate(0) == 1) return base;
    return std::make_shared<Power>(base, exponent);
}

ExprPtr Add::derivative() {
    return makeAdd(left->derivative(), right->derivative());
}

ExprPtr Multiply::derivative() {
    return makeAdd(makeMul(left->derivative(), right),
                   makeMul(left, right->derivative()));
}

ExprPtr Power::derivative() {

    if (right->isConstant()) {
        double n = right->evaluate(0);
        return makeMul(makeMul(std::make_shared<Constant>(n),
                               makePow(left, std::make_shared<Constant>(n - 1))),
                       left->derivative());
    }

    ExprPtr self = std::make_shared<Power>(left, right);
    ExprPtr term1 = makeMul(right->derivative(), std::make_shared<Ln>(left));
    ExprPtr term2 = makeMul(makeMul(right, left->derivative()),
                            makePow(left, std::make_shared<Constant>(-1)));
    return makeMul(self, makeAdd(term1, term2));
}

ExprPtr Sin::derivative() {
    return makeMul(std::make_shared<Cos>(arg), arg->derivative());
}

ExprPtr Cos::derivative() {
    return makeMul(makeMul(std::make_shared<Constant>(-1), std::make_shared<Sin>(arg)),
                   arg->derivative());
}

ExprPtr Ln::derivative() {
    return makeMul(arg->derivative(), makePow(arg, std::make_shared<Constant>(-1)));
}

int main() {
    auto x = std::make_shared<Variable>();
    auto expr =
        std::make_shared<Multiply>(
            std::make_shared<Add>(
                std::make_shared<Power>(
                    x,
                    std::make_shared<Constant>(2)
                ),
                std::make_shared<Multiply>(
                    std::make_shared<Constant>(3),
                    x
                )
            ),
            std::make_shared<Sin>(x)
        );
    auto deriv = expr->derivative();
    std::cout << "f(x) = " << expr->toString() << std::endl;
    std::cout << "f'(x) = " << deriv->toString() << std::endl;
    std::cout << "f(2) = " << expr->evaluate(2) << std::endl;
}
