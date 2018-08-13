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

#pragma once

#ifndef BKCMR_SAMPLING_SPHERE_SECTION_H
#define BKCMR_SAMPLING_SPHERE_SECTION_H

#include <memory>
#include <vector>

#include <bkMath/Matrix>

#include "../lib/export.h"

namespace bk
{
  [[nodiscard]] double BK_DATASET_EXPORT distance_on_sphere(double theta0, double phi0, double theta1, double phi1, double radius = 1);
  void BK_DATASET_EXPORT poisson_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360, double minDistOnSphere = 0.1, unsigned int maxTriesPerSample = 1000);
  void BK_DATASET_EXPORT poisson_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360, double minDistOnSphere = 0.1, unsigned int maxTriesPerSample = 1000);
  void BK_DATASET_EXPORT poisson_sphere(std::vector<bk::Vec3d>& samples_out, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360, double minDistOnSphere = 0.1, unsigned int maxTriesPerSample = 1000);
  void BK_DATASET_EXPORT uniform_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, unsigned int numSamples = 250, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360);
  void BK_DATASET_EXPORT uniform_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, unsigned int numSamples = 250, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360);
  void BK_DATASET_EXPORT random_sphere(std::vector<double>& sampleTheta_out, std::vector<double>& samplePhi_out, unsigned int numSamples = 250, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360);
  void BK_DATASET_EXPORT random_sphere(std::vector<double>& sampleX_out, std::vector<double>& sampleY_out, std::vector<double>& sampleZ_out, unsigned int numSamples = 250, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360);
  void BK_DATASET_EXPORT random_sphere(std::vector<bk::Vec3d>& samples_out, unsigned int numSamples = 250, double thetaMin_deg = 0, double thetaMax_deg = 60, double phiMin_deg = 0, double phiMax_deg = 360);
} // namespace bk

#endif //BKCMR_SAMPLING_SPHERE_SECTION_H


