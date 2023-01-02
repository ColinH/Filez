// Copyright (c) 2022-2023 Dr. Colin Hirsch - All Rights Reserved

#pragma once

#include <type_traits>

#include "file_info_list.hpp"

namespace filez
{
   class find_duplicates_base
   {
   public:
      virtual void add( const file_info_list& ) = 0;
      virtual void work() = 0;

   protected:
      find_duplicates_base() noexcept = default;
      ~find_duplicates_base() = default;

      find_duplicates_base( find_duplicates_base&& ) = delete;
      find_duplicates_base( const find_duplicates_base& ) = delete;

      void operator=( find_duplicates_base&& ) = delete;
      void operator=( const find_duplicates_base& ) = delete;
   };

   template< typename T >
   class find_duplicates final
      : public find_duplicates_base
   {
   public:
      find_duplicates() noexcept( std::is_nothrow_default_constructible_v< T > ) = default;

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
