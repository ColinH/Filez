// Copyright (c) 2022-2025 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <cstdint>

namespace filez
{
   struct deduplicate_args
   {
      bool h = false;
      bool H = false;

      std::size_t c = 0;

      [[nodiscard]] bool valid() const noexcept
      {
         return h != H;
      }
   };

}  // namespace filez
