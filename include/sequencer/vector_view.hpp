#pragma once

#include <type_traits>
#include <vector>

namespace sequencer
{
    template < class T, class View, class Vector >
    class vector_view_base
    {
    public:
        using size_type = typename Vector::size_type;
        using value_type = T;

        constexpr explicit vector_view_base( Vector* v, size_type offset,
                                             size_type stride = 1 ) noexcept
            : v_( v ), offset_( offset ), stride_( stride )
        {
        }

        constexpr explicit vector_view_base( View* view, size_type offset,
                                             size_type stride = 1 ) noexcept
            : view_( view ), offset_( offset ), stride_( stride )
        {
        }

        const T& operator[]( size_type i ) const
        {
            return view_ ? ( *view_ )[ idx( i ) ] : ( *v_ )[ idx( i ) ];
        }

        size_type size() const noexcept
        {
            const auto full_size = view_ ? view_->size() : v_->size();
            return full_size / stride_ - offset_ / stride_;
        }

    protected:
        constexpr size_type idx( size_type i ) const noexcept
        {
            return offset_ + stride_ * i;
        }

        Vector* vector() noexcept
        {
            return v_;
        }

        View* view() noexcept
        {
            return view_;
        }

    private:
        Vector* v_{nullptr};
        View* view_{nullptr};
        const size_type offset_;
        const size_type stride_;
    };

    template < class T >
    class const_vector_view
        : public vector_view_base< T, const const_vector_view< T >, const std::vector< T > >
    {
    public:
        using base_type =
            vector_view_base< T, const const_vector_view< T >, const std::vector< T > >;
        using size_type = typename base_type::size_type;
        using value_type = typename base_type::value_type;

        constexpr explicit const_vector_view( const std::vector< T >* v, size_type offset,
                                              size_type stride = 1 ) noexcept
            : base_type{v, offset, stride}
        {
        }

        constexpr explicit const_vector_view( const const_vector_view* view, size_type offset,
                                              size_type stride = 1 ) noexcept
            : base_type{view, offset, stride}
        {
        }
    };

    template < class T >
    class vector_view : public vector_view_base< T, vector_view< T >, std::vector< T > >
    {
    public:
        using base_type = vector_view_base< T, vector_view< T >, std::vector< T > >;
        using size_type = typename base_type::size_type;
        using value_type = typename base_type::value_type;

        constexpr explicit vector_view( std::vector< T >* v, size_type offset,
                                        size_type stride = 1 ) noexcept
            : base_type{v, offset, stride}
        {
        }

        constexpr explicit vector_view( vector_view* view, size_type offset,
                                        size_type stride = 1 ) noexcept
            : base_type{view, offset, stride}
        {
        }

        using base_type::operator[];

        T& operator[]( size_type i )
        {
            return base_type::view() ? ( *base_type::view() )[ base_type::idx( i ) ]
                                     : ( *base_type::vector() )[ base_type::idx( i ) ];
        }
    };
} // namespace sequencer
