// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

namespace filez
{
   struct deduplicate_args
   {
      bool h = false;
      bool H = false;

      [[nodiscard]] bool valid() const noexcept
      {
         return h != H;
      }
   };

}  // namespace filez
