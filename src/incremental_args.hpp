// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

namespace filez
{
   struct incremental_args
   {
      bool h = false;
      bool H = false;
      bool n = false;
      bool N = false;
      bool p = false;
      bool P = false;

      [[nodiscard]] bool is_any_set() const noexcept
      {
         return h || H || n || N || p || P;
      }
   };

}  // namespace filez
