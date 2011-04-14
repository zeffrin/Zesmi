
#include "vector.hpp"


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
