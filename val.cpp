#include "val.h"

Value Value::operator&&(const Value& oper) const
{
    if (this->IsInt() || this->IsErr() || this->IsReal() || oper.IsInt() || oper.IsErr() || oper.IsReal()) {
        return Value();
    }

    if (this->IsBool() && oper.IsBool()) {
        return Value(this->GetBool() && oper.GetBool());
    }

    return Value();
}

Value Value::operator<(const Value& oper) const
{
    if (this->IsString() || this->IsErr() || oper.IsString() || oper.IsErr()) {
        return Value();
    }

    if (this->IsBool() && oper.IsBool()) {
        return Value(this->GetBool() < oper.GetBool());
    }

    if (this->IsInt() && oper.IsInt()) {
        return Value(this->GetInt() < oper.GetInt());
    }

    if (this->IsReal() && oper.IsReal()) {
        return Value(this->GetReal() < oper.GetReal());
    }

    if (this->IsReal() && oper.IsInt()) {
        return Value(this->GetReal() < oper.GetInt());
    }

    if (this->IsInt() && oper.IsReal()) {
        return Value(this->GetInt() < oper.GetReal());
    }

    return Value();
}

Value Value::operator-(const Value& oper) const
{
    if (this->IsBool() || this->IsErr() || this->IsString() || oper.IsBool() || oper.IsErr() || oper.IsString()) {
        return Value();
    }

    if (this->IsInt() && oper.IsInt()) {
        return Value(this->GetInt() - oper.GetInt());
    }

    if (this->IsReal() && oper.IsReal()) {
        return Value(this->GetReal() - oper.GetReal());
    }

    if (this->IsReal() && oper.IsInt()) {
        return Value(this->GetReal() - oper.GetInt());
    }

    if (this->IsInt() && oper.IsReal()) {
        return Value(this->GetInt() - oper.GetReal());
    }

    return Value();
}

Value Value::operator*(const Value& oper) const
{
    if (this->IsBool() || this->IsErr() || this->IsString() || oper.IsBool() || oper.IsErr() || oper.IsString()) {
        return Value();
    }

    if (this->IsInt() && oper.IsInt()) {
        return Value(this->GetInt() * oper.GetInt());
    }

    if (this->IsReal() && oper.IsReal()) {
        return Value(this->GetReal() * oper.GetReal());
    }

    if (this->IsReal() && oper.IsInt()) {
        return Value(this->GetReal() * oper.GetInt());
    }

    if (this->IsInt() && oper.IsReal()) {
        return Value(this->GetInt() * oper.GetReal());
    }

    return Value();
}

Value Value::operator+(const Value& oper) const{
	if(this->IsBool() || this->IsErr() || this->IsString()  || oper.IsBool() || oper.IsErr() || oper.IsString()){
		return Value();
	}
	if (this->IsInt() && oper.IsInt()) {
        return Value(this->GetInt() + oper.GetInt());
    }

    if (this->IsReal() && oper.IsReal()) {
        return Value(this->GetReal() + oper.GetReal());
    }

    if (this->IsReal() && oper.IsInt()) {
        return Value(this->GetReal() + oper.GetInt());
    }

    if (this->IsInt() && oper.IsReal()) {
        return Value(this->GetInt() + oper.GetReal());
    }
	return Value();
}

Value Value::operator>(const Value& oper) const{
    if (this->IsString() || this->IsErr() || oper.IsString() || oper.IsErr()) {
        return Value();
    }

    if (this->IsBool() && oper.IsBool()) {
        return Value(this->GetBool() > oper.GetBool());
    }

    if (this->IsInt() && oper.IsInt()) {
        return Value(this->GetInt() > oper.GetInt());
    }

    if (this->IsReal() && oper.IsReal()) {
        return Value(this->GetReal() > oper.GetReal());
    }

    if (this->IsReal() && oper.IsInt()) {
        return Value(this->GetReal() > oper.GetInt());
    }

    if (this->IsInt() && oper.IsReal()) {
        return Value(this->GetInt() > oper.GetReal());
    }

    return Value();	
}

Value Value::operator==(const Value& oper) const
{
	if (this->IsErr() || oper.IsErr()) {
        return Value();
    }
	if (this->IsReal() && oper.IsReal()){
		return Value(this->GetReal() == oper.GetReal());
	}
	if(this->IsInt() && oper.IsInt()){
		return Value(this->GetReal() == oper.GetInt());
	}
    if (this->IsBool() && oper.IsBool()) {
        return Value(this->GetBool() == oper.GetBool());
    }

    return Value();
}

Value Value::operator||(const Value& oper) const
{
	if (this->IsInt() || this->IsErr() || this->IsReal() || oper.IsInt() || oper.IsErr() || oper.IsReal()) {
        return Value();
    }

    if (this->IsBool() && oper.IsBool()) {
        return Value(this->GetBool() || oper.GetBool());
    }

    return Value();
}

Value Value::operator*(const Value& oper) const
{

}

Value Value::operator/(const Value& oper) const
{

}

Value Value::operator!() const{
	if(!this->IsBool()){
		return Value();
	}
	return Value(!this->GetBool());
}