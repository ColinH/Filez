// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <type_traits>

#include "file_info_list.hpp"

namespace filez
{
   class find_variations_base
   {
   public:
      virtual void add( const file_info_list& ) = 0;
      virtual void work() = 0;

   protected:
      find_variations_base() noexcept = default;
      ~find_variations_base() = default;

      find_variations_base( find_variations_base&& ) = delete;
      find_variations_base( const find_variations_base& ) = delete;

      void operator=( find_variations_base&& ) = delete;
      void operator=( const find_variations_base& ) = delete;
   };

   template< typename T >
   class find_variations final
      : public find_variations_base
   {
   public:
      find_variations() noexcept( std::is_nothrow_default_constructible_v< T > ) = default;

      void add( const file_info_list& list ) override
      {
         m_t.add( list );
      }

      void work() override
      {
         m_t.work();
      }

   private:
      T m_t;
   };

}  // namespace filez
