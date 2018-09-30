#pragma once

#ifndef BK_EVESSELSEMANTIC_H
#define BK_EVESSELSEMANTIC_H

namespace bk
{
  inline namespace cmr
  {
    enum VesselSemantic_ : unsigned int
    {
        VesselSemantic_None = 0, //
        VesselSemantic_LeftHeart_Aorta = 1, //
        VesselSemantic_LeftHeart_Ventricle = 1 << 1, //
        VesselSemantic_LeftHeart_Atrium = 1 << 2, //
        VesselSemantic_LeftHeart_PulmonaryVeins = 1 << 3, //
        VesselSemantic_RightHeart_PulmonaryArtery = 1 << 4, //
        VesselSemantic_RightHeart_Ventricle = 1 << 5, //
        VesselSemantic_RightHeart_Atrium = 1 << 6, //
        VesselSemantic_RightHeart_VenaCava = 1 << 7, //
    };
  } // inline namespace cmr
} // namespace bk

#endif //BK_EVESSELSEMANTIC_H
