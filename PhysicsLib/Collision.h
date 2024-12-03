// collision.h

#ifndef COLLISION_H
#define COLLISION_H

struct AABB {
    double x;
    double y;
    double width;
    double height;
};

inline bool CheckCollision(const AABB& a, const AABB& b) {
    return !(a.x + a.width < b.x || a.x > b.x + b.width ||
             a.y + a.height < b.y || a.y > b.y + b.height);
}

#endif // COLLISION_H
