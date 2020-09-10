#include "View.h"
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include "Chunk/Chunk.h"
#include "inputs.h"

static glm::mat<4, 4, double, glm::packed_highp> viewMatrix;
static glm::mat<4, 4, double, glm::packed_highp> projectionMatrix;
static std::vector<glm::vec2> hull;

static std::vector<glm::vec2> calculateHull(std::vector<glm::vec2> points)
{
    if (points.size() < 3)
        throw std::invalid_argument("Convex hull not possible with less than 3 points.");

    std::vector<glm::vec2> hull;

    const auto sortByX = [](const glm::vec2& a, const glm::vec2& b) { return a.x < b.x; };

    std::sort(points.begin(), points.end(), sortByX);

    glm::vec2* leftMost = &points[0];
    glm::vec2* current = leftMost;
    glm::vec2* compare = &points[1];

    for (int i = 1; *current == *compare; i++)
    {
        if(i == points.size())
            throw std::invalid_argument("Atlest 3 unique points needed for convex hull");
        compare = &points[1 + i];
    }
    do
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (points[i] != *compare && points[i] != *current)
            {
                float crossZ = ((points[i].y - current->y) * (compare->x - current->x) -
                    (compare->y - current->y) * (points[i].x - current->x));
                if (crossZ > 0)
                {
                    compare = &points[i];
                }
            }
        }
        current = compare;
        compare = &points[0];
        hull.push_back(*current);
    }
    while (current != leftMost);

    return hull;
}

void ViewFrustum::initialize()
{
    viewMatrix = glm::lookAt(Camera::GetPosition(), Camera::GetPosition() + Camera::GetCameraAngle(), glm::vec<3, double, glm::packed_highp>(0.0f, 1.0f, 0.0f));
    projectionMatrix = glm::perspective(glm::radians(Settings::fov), 16.0f/9.0f, Settings::ZNEAR, Settings::ZFAR);
    hull.reserve(8);
}

glm::mat<4, 4, double, glm::packed_highp> ViewFrustum::getViewMatrix()
{
	return viewMatrix;
}

glm::mat<4, 4, double, glm::packed_highp> ViewFrustum::getProjMatrix()
{
    return projectionMatrix;
}

static std::vector<glm::vec2> calculateCameraPointsAndCastTo2D()
{
    std::vector<glm::vec2> points;
    points.reserve(8);

    auto viewInverse = glm::inverse(glm::lookAt(Camera::GetPosition() - Camera::GetCameraAngle() * 32.0, Camera::GetPosition(), glm::vec<3, double, glm::packed_highp>(0.0f, 1.0f, 0.0f)));
    auto projInverse = glm::inverse(projectionMatrix);

    glm::vec<4, double, glm::packed_highp> Point1 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>( 1.0,  1.0,  1.0, 1.0);
    Point1 /= Point1.w;                                         
    glm::vec<4, double, glm::packed_highp> Point2 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>( 1.0,  1.0, -1.0, 1.0); //
    Point2 /= Point2.w;                                           
    glm::vec<4, double, glm::packed_highp> Point3 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>( 1.0, -1.0,  1.0, 1.0);
    Point3 /= Point3.w;                            
    glm::vec<4, double, glm::packed_highp> Point4 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>( 1.0, -1.0, -1.0, 1.0); //
    Point4 /= Point4.w;                         
    glm::vec<4, double, glm::packed_highp> Point5 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>(-1.0,  1.0,  1.0, 1.0);
    Point5 /= Point5.w;                               
    glm::vec<4, double, glm::packed_highp> Point6 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>(-1.0,  1.0, -1.0, 1.0); //
    Point6 /= Point6.w;                              
    glm::vec<4, double, glm::packed_highp> Point7 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>(-1.0, -1.0,  1.0, 1.0); //1
    Point7 /= Point7.w;                            
    glm::vec<4, double, glm::packed_highp> Point8 = viewInverse * projInverse * glm::vec<4, double, glm::packed_highp>(-1.0, -1.0, -1.0, 1.0); //2
    Point8 /= Point8.w;
    //point7den  point8e giden line,, point7.x + (point8.x - point7.x)t, point7.y + (point8.y - point7.y)t, point7.z + (point8.z - point7.z)t;
    //if point8.y < 0 -> point7.y + (point8.y - point7.y)t=0 -> t = point7.y/(point7.y - point8.y) -> p78 = point8-point7 -> p78 = p78 * t
    // p78 = p78 * (point7.y/(point7.y - point8.y)) = p78 = (point8-point7) * (point7.y/(point7.y - point8.y)) = 
    // = ((p8.x-p7.x) * p7.y/(p7.y-p8.y), -p7.y, (p8.z-p7.z) * p7.y/(p7.y-p8.y))
    // + p7 = (((p8.x-p7.x) * p7.y/(p7.y-p8.y)) + p7.x, 0, ((p8.z-p7.z) * p7.y/(p7.y-p8.y)) + p7.z) = p8

    //
    //glm::vec3 D = glm::vec3(Point7 - Point8);
    //glm::vec3 N = glm::vec3(0.0f, 1.0f, 0.0f);
    //Point7 = glm::vec4(glm::vec3(Point8) + D * glm::dot(glm::vec3(0.0f) - glm::vec3(Point8), N) / glm::dot(D, N), 1.0f);

    //p8.y > 256 -> point7.y + (point8.y - point7.y)t= 256 -> (256 - p7.y)/(point8.y - point7.y) = t
    // a = (256 - p7.y)/(point8.y - point7.y)
    // p78 = p8-p7, p78 = p78 * t = (p8-p7) * (256 - p7.y)/(point8.y - point7.y) = ( (p8.x-p7.x) * a, (p8.y-p7.y) * a, (p8.z-p7.z) * a )
    // p78 + p7 = vec4( ((p8.x-p7.x) * a) + p7.x, ((p8.y-p7.y) * a) + p7.y, ((p8.z-p7.z) * a) + p7.z ) 
    //((p8.y - p7.y) * a) + p7.y = 256
    // p78 + p7 = vec4( ((p8.x-p7.x) * a) + p7.x, 256, ((p8.z-p7.z) * a) + p7.z ) 
    // a = (256 - p7.y)/(point8.y - point7.y)

    //Ycap
    // a = (cap - p7.y)/(point8.y - point7.y)
    // p78 + p7 = vec4( ((p8.x-p7.x) * a) + p7.x, cap, ((p8.z-p7.z) * a) + p7.z )

    if(Camera::GetCameraAngle().y > 0)
    {
        if(Point1.y > CHUNK_HEIGHT)
        {
            float yesMan = (CHUNK_HEIGHT - Point2.y) / (Point1.y - Point2.y);
            Point1 = glm::vec4((Point1.x - Point2.x) * yesMan + Point2.x, CHUNK_HEIGHT, (Point1.z-Point2.z) * yesMan + Point2.z, 1.0f);
	    }
        if(Point3.y > CHUNK_HEIGHT)
        {
            float yesMan = (CHUNK_HEIGHT - Point4.y) / (Point3.y - Point4.y);
            Point3 = glm::vec4((Point3.x - Point4.x) * yesMan + Point4.x, CHUNK_HEIGHT, (Point3.z-Point4.z) * yesMan + Point4.z, 1.0f);
	    }
        if(Point5.y > CHUNK_HEIGHT)
        {
            float yesMan = (CHUNK_HEIGHT - Point6.y) / (Point5.y - Point6.y);
            Point5 = glm::vec4((Point5.x - Point6.x) * yesMan + Point6.x, CHUNK_HEIGHT, (Point5.z-Point6.z) * yesMan + Point6.z, 1.0f);
	    }
        if (Point7.y > CHUNK_HEIGHT)
        {
            float yesMan = (CHUNK_HEIGHT - Point8.y) / (Point7.y - Point8.y);
            Point7 = glm::vec4((Point7.x - Point8.x) * yesMan + Point8.x, CHUNK_HEIGHT, (Point7.z - Point8.z) * yesMan + Point8.z, 1.0f);
        }
	}
    else
    {
        if(Point1.y < 0)
        {
            float yesMan = Point2.y / (Point2.y - Point1.y);
            Point1 = glm::vec4(((Point1.x - Point2.x) * yesMan) + Point2.x, 0, ((Point1.z - Point2.z) * yesMan) + Point2.z, 1.0f);
	    }
        if (Point3.y < 0)
        {
            float yesMan = Point4.y / (Point4.y - Point3.y);
            Point3 = glm::vec4(((Point3.x - Point4.x) * yesMan) + Point4.x, 0, ((Point3.z - Point4.z) * yesMan) + Point4.z, 1.0f);
        }
        if (Point5.y < 0)
        {
            float yesMan = Point6.y / (Point6.y - Point5.y);
            Point5 = glm::vec4(((Point5.x - Point6.x) * yesMan) + Point6.x, 0, ((Point5.z - Point6.z) * yesMan) + Point6.z, 1.0f);
        }
        if(Point7.y < 0)
        {
            float yesMan = Point8.y / (Point8.y - Point7.y);
            Point7 = glm::vec4(((Point7.x - Point8.x) * yesMan) + Point8.x, 0, ((Point7.z - Point8.z) * yesMan) + Point8.z, 1.0f);
	    }
	}

    /*

    The ray is defined by a point R0 and a direction D.
    The plane is defined by a triangle with the three points PA, PB, and PC.

    The normal vector of the plane can be calculated by the cross product of 2 legs of the triangle:

    N  =  cross(PC-PA, PB-PA)
    The normal distance n of the point R0 to the plane is:

    n  =  | R0 - PA | * cos(alpha)  =  dot(PA - R0, N)
    It follows that the distance d of the intersection point X to the origin of the ray R0 is:

    d  =  n / cos(beta)  =  n / dot(D, N)
    The intersection point X is:

    X  =  R0 + D * d  =  R0 + D * dot(PA - R0, N) / dot(D, N)
    */
   // std::cout << Point7.x << " " << Point7.y << " "<< Point7.z << "\n";
   // std::cout << Point5.x << " " << Point5.y << " " << Point5.z << "\n";
   // std::cout << Point3.x << " " << Point3.y << " " << Point3.z << "\n";
   // std::cout << Point1.x << " " << Point1.y << " " << Point1.z << "\n\n\n";


    points.push_back(glm::vec2(Point1.x, Point1.z));
    points.push_back(glm::vec2(Point2.x, Point2.z));
    points.push_back(glm::vec2(Point3.x, Point3.z));
    points.push_back(glm::vec2(Point4.x, Point4.z));
    points.push_back(glm::vec2(Point5.x, Point5.z));
    points.push_back(glm::vec2(Point6.x, Point6.z));
    points.push_back(glm::vec2(Point7.x, Point7.z));
    points.push_back(glm::vec2(Point8.x, Point8.z));

    return points; 
}

void ViewFrustum::update()
{
	viewMatrix = glm::lookAt(Camera::GetPosition(), Camera::GetPosition() + Camera::GetCameraAngle(), glm::vec<3, double, glm::packed_highp>(0.0f, 1.0f, 0.0f));
    hull = calculateHull(calculateCameraPointsAndCastTo2D());
}

bool ViewFrustum::contains2D(glm::vec2 pos)
{
    for(int i = 0; i < hull.size(); i++)
    {
        auto a = pos - hull[i];
        auto b = hull[(i + 1) % hull.size()] - hull[i];
        float crossZ = a.x * b.y - a.y * b.x;
        if(crossZ < 0)
        {
            return false;
		}
	}
	return true;
}
