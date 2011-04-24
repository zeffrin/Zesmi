
#include "vector.hpp"


bool Vector::operator==(const Vector &rhs) const
{
    if(this == &rhs) return true;
    if(this->X == rhs.X &&
       this->Y == rhs.Y &&
       this->Z == rhs.Z &&
       this->Pitch == rhs.Pitch &&
       this->Yaw == rhs.Yaw) { return true; }

    return false;
}

bool Vector::operator!=(const Vector &rhs) const
{
    return !(*this == rhs);
}

Vector& Vector::operator=(const Vector &rhs)
{

    // Only do assignment if RHS is a different object from this.
    if (this != &rhs) {
      this->X = rhs.X;
      this->Y = rhs.Y;
      this->Z = rhs.Z;
      this->Pitch = rhs.Pitch;
      this->Yaw = rhs.Yaw;
    }

    return *this;
  }

Vector& Vector::operator+=(const Vector &rhs)
{
    // Only do assignment if RHS is a different object from this.
    if (this != &rhs) {
      this->X += rhs.X;
      this->Y += rhs.Y;
      this->Z += rhs.Z;
      this->Pitch += rhs.Pitch;
      this->Yaw += rhs.Yaw;
    }
    return *this;
}

const Vector Vector::operator+(const Vector &rhs) const
{
    return Vector(*this) += rhs;
}

Vector& Vector::operator-=(const Vector &rhs)
{
    // Only do assignment if RHS is a different object from this.
    if (this != &rhs) {
      this->X -= rhs.X;
      this->Y -= rhs.Y;
      this->Z -= rhs.Z;
      this->Pitch -= rhs.Pitch;
      this->Yaw -= rhs.Yaw;
    }
    return *this;
}

const Vector Vector::operator-(const Vector &rhs) const
{
    return Vector(*this) -= rhs;
}


bool Position::operator==(const Position &rhs) const
{
    if(this == &rhs) return true;
    if(this->X == rhs.X &&
       this->Y == rhs.Y &&
       this->Z == rhs.Z) { return true; }

    return false;
}

bool Position::operator!=(const Position &rhs) const
{
    return !(*this == rhs);
}

Position& Position::operator=(const Position &rhs)
{
    if (this != &rhs) {
      this->X = rhs.X;
      this->Y = rhs.Y;
      this->Z = rhs.Z;
    }

    return *this;
}

Position& Position::operator+=(const Position &rhs)
{
    // Only do assignment if RHS is a different object from this.
    if (this != &rhs) {
      this->X += rhs.X;
      this->Y += rhs.Y;
      this->Z += rhs.Z;
    }

    return *this;
}

const Position Position::operator+(const Position &rhs) const
{
    return Position(*this) += rhs;
}

Position& Position::operator-=(const Position &rhs)
{
    // Only do assignment if RHS is a different object from this.
    if (this != &rhs) {
      this->X -= rhs.X;
      this->Y -= rhs.Y;
      this->Z -= rhs.Z;
    }

    return *this;
}

const Position Position::operator-(const Position &rhs) const
{
    return Position(*this) -= rhs;
}
