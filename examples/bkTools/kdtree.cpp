/*
 * MIT License
 *
 * Copyright (c) 2018 Benjamin Köhler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include <bk/KDTree>

/*
 * custom point class
 */
struct Pt2
{
    using value_type = double;
    value_type x[2];

    double& operator[](unsigned int i)
    { return x[i]; }

    double operator[](unsigned int i) const
    { return x[i]; }
};


int main(int, char**)
{
    std::cout.precision(2);
    std::cout << std::fixed;

    
    std::mt19937_64 generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> distribution(-10, 10);
    
    using point_type = Pt2;

    /*
     * create random points
     */

    std::cout << "points" << std::endl;
    std::cout << "------" << std::endl;

    constexpr unsigned int N = 10;
    std::vector<point_type> points(N);
    for (unsigned int i = 0; i < N; ++i)
    {
        point_type& p = points[i];

        p[0] = distribution(generator);
        p[1] = distribution(generator);

        std::cout << "p" << i << ": " << std::setw(5) << p[0] << " " << std::setw(5) << p[1] << " (dist to origin " << std::setw(4) << std::sqrt(p[0] * p[0] + p[1] * p[1]) << ")" << std::endl;
    }

    /*
     * construct KDTree
     */
    bk::KDTree<point_type, /*points' value type =*/ double> kd;
    // bk::KDTree<point_type> kd; // uses point_type::value_type as second template parameter

    kd.construct(points.begin(), points.end(), /*numDimensions =*/ 2);

    /*
     * use KDTree
     */
    const point_type query{0, 0};


    /*
     * nearest neighbor search
     */
    std::cout << std::endl;
    std::cout << "nearest neighbor search" << std::endl;
    std::cout << "-----------------------" << std::endl;

    bk::KDPointInfo<point_type> cp = kd.nearest_neighbor(query);

    std::cout << "closest point to (0,0):" << std::endl;
    std::cout << "p" << cp.point_id << ": " << std::setw(5) << cp.point[0] << " " << std::setw(5) << cp.point[1] << " (dist to query " << std::setw(4) << cp.distance_to_query << ")" << std::endl;


    /*
     * radius search
     */
    std::cout << std::endl;
    std::cout << "radius search" << std::endl;
    std::cout << "-------------" << std::endl;

    std::vector<bk::KDPointInfo<point_type>> points_within_radius = kd.neighbors_within_radius(query, 5);

    std::cout << "points within radius = 5 to (0,0):" << std::endl;
    for (const bk::KDPointInfo<point_type>& c: points_within_radius)
    { std::cout << "p" << c.point_id << ": " << std::setw(5) << c.point[0] << " " << std::setw(5) << c.point[1] << " (dist to query " << std::setw(4) << c.distance_to_query << ")" << std::endl; }

    /*
     * k nearest neighbor search with k = 5
     */
    std::cout << std::endl;
    std::cout << "k nearest neighbor search with k = 5" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    std::vector<bk::KDPointInfo<point_type>> k_nearest_points = kd.k_nearest_neighbors(query, 5);

    std::cout << "k=5 nearest neighbors:" << std::endl;
    for (const bk::KDPointInfo<point_type>& c: k_nearest_points)
    { std::cout << "p" << c.point_id << ": " << std::setw(5) << c.point[0] << " " << std::setw(5) << c.point[1] << " (dist to query " << std::setw(4) << c.distance_to_query << ")" << std::endl; }

    return EXIT_SUCCESS;
}

