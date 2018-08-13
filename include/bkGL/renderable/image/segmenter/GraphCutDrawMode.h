#pragma once

#ifndef BK_GRAPHCUTDRAWMODE_H
#define BK_GRAPHCUTDRAWMODE_H

namespace bk
{
  namespace details
  {
    enum class GraphCutDrawMode : int
    {
        Inside = 1, Outside = 2, Erase = 3
    };
  } // namespace details
} // namespace bk

#endif //BK_GRAPHCUTDRAWMODE_H


