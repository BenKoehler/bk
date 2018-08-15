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

#include <bkCMR/sampling/sampling_sphere_section.h>

#include <array>
#include <cmath>

#include <bk/Random>

namespace bk
{
  double distance_on_sphere(double theta0, double phi0, double theta1, double phi1, double radius)
  {
      const bk::Vec3d p0(std::sin(theta0) * std::cos(phi0), std::sin(theta0) * std::sin(phi0), std::cos(theta0));
      const bk::Vec3d p1(std::sin(theta1) * std::cos(phi1), std::sin(theta1) * std::sin(phi1), std::cos(theta1));
      const double d = p0.distance(p1);

      //const std::array<double, 2> x = {std::sin(theta0) * std::cos(phi0), std::sin(theta1) * std::cos(phi1)};
      //const std::array<double, 2> y = {std::sin(theta0) * std::sin(phi0), std::sin(theta1) * std::sin(phi1)};
      //const std::array<double, 2> z = {std::cos(theta0), std::cos(theta1)};
      //const double dx = x[0] - x[1];
      //const double dy = y[0] - y[1];
      //const double dz = z[0] - z[1];
      //const double d = std::sqrt(dx * dx + dy * dy + dz * dz);

      return radius * std::asin(d / (2.0 * radius) * std::sqrt(4.0 * radius * radius - d * d));
  }

  void poisson_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg, double minDistOnSphere, unsigned int maxTriesPerSample)
  {
      const double thetaMin = degrees_to_radians(thetaMin_deg);
      const double thetaMax = degrees_to_radians(thetaMax_deg);
      const double phiMin = degrees_to_radians(phiMin_deg);
      const double phiMax = degrees_to_radians(phiMax_deg);
      bool success = true;
      double theta = 0;
      double phi = 0;

      while (success)
      {
          for (unsigned int k = 0; k < maxTriesPerSample; ++k)
          {
              success = true;
              const double u = bk::random::make_double(0,1);
              const double v = bk::random::make_double(0,1);
              theta = std::acos(std::cos(thetaMin) + u * (std::cos(thetaMax) - std::cos(thetaMin)));
              phi = phiMin + v * (phiMax - phiMin);

              for (unsigned int i = 0; i < sampleTheta_out.size(); ++i)
              {
                  const double d = distance_on_sphere(theta, phi, sampleTheta_out[i], samplePhi_out[i]);
                  success = success && (d >= minDistOnSphere);
                  if (!success)
                  { break; }
              }

              if (success)
              { break; }
          } // for k : maxTriesPerSample

          if (success)
          {
              sampleTheta_out.push_back(theta);
              samplePhi_out.push_back(phi);
          }
      } // while (success)
  }

  void poisson_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg, double minDistOnSphere, unsigned int maxTriesPerSample)
  {
      std::vector<double> sampleTheta;
      std::vector<double> samplePhi;
      poisson_sphere(sampleTheta, samplePhi, thetaMin_deg, thetaMax_deg, phiMin_deg, phiMax_deg, minDistOnSphere, maxTriesPerSample);

      typename std::vector<double>::iterator itTheta = std::begin(sampleTheta);
      typename std::vector<double>::iterator itPhi = std::begin(samplePhi);
      for (; itTheta != std::end(sampleTheta); ++itTheta, ++itPhi)
      {
          sampleX_out.push_back(std::sin(*itTheta) * std::cos(*itPhi));
          sampleY_out.push_back(std::sin(*itTheta) * std::sin(*itPhi));
          sampleZ_out.push_back(std::cos(*itTheta));
      }
  }

  void poisson_sphere(std::vector<bk::Vec3d>& samples_out, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg, double minDistOnSphere, unsigned int maxTriesPerSample)
  {
      std::vector<double> sampleTheta;
      std::vector<double> samplePhi;
      poisson_sphere(sampleTheta, samplePhi, thetaMin_deg, thetaMax_deg, phiMin_deg, phiMax_deg, minDistOnSphere, maxTriesPerSample);

      typename std::vector<double>::iterator itTheta = std::begin(sampleTheta);
      typename std::vector<double>::iterator itPhi = std::begin(samplePhi);
      for (; itTheta != std::end(sampleTheta); ++itTheta, ++itPhi)
      { samples_out.push_back(bk::Vec3d(std::sin(*itTheta) * std::cos(*itPhi), std::sin(*itTheta) * std::sin(*itPhi), std::cos(*itTheta))); }
  }

  void uniform_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, unsigned int numSamples, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg)
  {
      const double thetaMin = degrees_to_radians(thetaMin_deg);
      const double thetaMax = degrees_to_radians(thetaMax_deg);
      const double phiMin = degrees_to_radians(phiMin_deg);
      const double phiMax = degrees_to_radians(phiMax_deg);
      double theta = 0;
      double phi = 0;

      for (unsigned int k = 0; k < numSamples; ++k)
      {
          const double u = bk::random::make_double(0,1);
          const double v = bk::random::make_double(0,1);
          theta = std::acos(std::cos(thetaMin) + u * (std::cos(thetaMax) - std::cos(thetaMin)));
          phi = phiMin + v * (phiMax - phiMin);

          sampleTheta_out.push_back(theta);
          samplePhi_out.push_back(phi);
      }
  }

  void uniform_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, unsigned int numSamples, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg)
  {
      std::vector<double> sampleTheta;
      std::vector<double> samplePhi;
      uniform_sphere(sampleTheta, samplePhi, numSamples, thetaMin_deg, thetaMax_deg, phiMin_deg, phiMax_deg);

      typename std::vector<double>::iterator itTheta = std::begin(sampleTheta);
      typename std::vector<double>::iterator itPhi = std::begin(samplePhi);
      for (; itTheta != std::end(sampleTheta); ++itTheta, ++itPhi)
      {
          sampleX_out.push_back(std::sin(*itTheta) * std::cos(*itPhi));
          sampleY_out.push_back(std::sin(*itTheta) * std::sin(*itPhi));
          sampleZ_out.push_back(std::cos(*itTheta));
      }
  }

  void random_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, unsigned int numSamples, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg)
  {
      const double thetaMin = degrees_to_radians(thetaMin_deg);
      const double thetaMax = degrees_to_radians(thetaMax_deg);
      const double phiMin = degrees_to_radians(phiMin_deg);
      const double phiMax = degrees_to_radians(phiMax_deg);
      double theta = 0;
      double phi = 0;

      for (unsigned int k = 0; k < numSamples; ++k)
      {
          const double u = bk::random::make_double(0,1);
          const double v = bk::random::make_double(0,1);
          theta = thetaMin + u * (thetaMax - thetaMin);
          phi = phiMin + v * (phiMax - phiMin);

          sampleTheta_out.push_back(theta);
          samplePhi_out.push_back(phi);
      }
  }

  void random_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, unsigned int numSamples, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg)
  {
      std::vector<double> sampleTheta;
      std::vector<double> samplePhi;
      random_sphere(sampleTheta, samplePhi, numSamples, thetaMin_deg, thetaMax_deg, phiMin_deg, phiMax_deg);

      typename std::vector<double>::iterator itTheta = std::begin(sampleTheta);
      typename std::vector<double>::iterator itPhi = std::begin(samplePhi);
      for (; itTheta != std::end(sampleTheta); ++itTheta, ++itPhi)
      {
          sampleX_out.push_back(std::sin(*itTheta) * std::cos(*itPhi));
          sampleY_out.push_back(std::sin(*itTheta) * std::sin(*itPhi));
          sampleZ_out.push_back(std::cos(*itTheta));
      }
  }

  void random_sphere(std::vector<bk::Vec3d>& samples_out, unsigned int numSamples, double thetaMin_deg, double thetaMax_deg, double phiMin_deg, double phiMax_deg)
  {
      std::vector<double> sampleTheta;
      std::vector<double> samplePhi;
      random_sphere(sampleTheta, samplePhi, numSamples, thetaMin_deg, thetaMax_deg, phiMin_deg, phiMax_deg);

      typename std::vector<double>::iterator itTheta = std::begin(sampleTheta);
      typename std::vector<double>::iterator itPhi = std::begin(samplePhi);
      for (; itTheta != std::end(sampleTheta); ++itTheta, ++itPhi)
      { samples_out.push_back(bk::Vec3d(std::sin(*itTheta) * std::cos(*itPhi), std::sin(*itTheta) * std::sin(*itPhi), std::cos(*itTheta))); }
  }
} // namespace bk


