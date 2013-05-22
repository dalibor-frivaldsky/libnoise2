#pragma once


#include <noise/math/Math.hpp>




namespace noise
{
	
	namespace proto
	{

		namespace gaborSurface
		{

			template< typename ValueType >
			class ProjectionVector
			{

			private:

				typedef math::Math< ValueType >		M;

			public:

				ProjectionVector( const typename M::Vector4F& viewXV, const typename M::Vector4F& viewYV, const typename M::Vector4F& viewZV,
								  const typename M::Vector4F& pXV, const typename M::Vector4F& pYV, const typename M::Vector4F& pZV ):
				  view( viewXV, viewYV, viewZV ),
				  up( M::constOneF(), M::constZeroF(), M::constZeroF() ),
				  p( pXV, pYV, pZV )
				{
					left = view.cross( up );
					left.normalize();

					up = left.cross( view );

					view = -view;
				}

				void
				project( typename M::Vector4F& projXV, typename M::Vector4F& projYV, typename M::Vector4F& projZV )
				{
					Vec3	tmp( projXV, projYV, projZV );
					tmp = tmp - p;

					projXV = tmp.dot( left );
					projYV = tmp.dot( up );
					projZV = tmp.dot( view );
				}



			private:

				struct Vec3
				{

					Vec3():
					  x( M::constZeroF() ),
					  y( M::constZeroF() ),
					  z( M::constZeroF() )
					{}

					Vec3( const typename M::Vector4F& x, const typename M::Vector4F& y, const typename M::Vector4F& z ):
					  x( x ),
					  y( y ),
					  z( z )
					{}

					Vec3
					operator - ()
					{
						Vec3	v;

						v.x = M::multiply( M::constMinusOneF(), this->x );
						v.y = M::multiply( M::constMinusOneF(), this->y );
						v.z = M::multiply( M::constMinusOneF(), this->z );

						return v;
					}

					Vec3
					operator - ( const Vec3& right )
					{
						Vec3	v;

						v.x = M::subtract( this->x, right.x );
						v.y = M::subtract( this->y, right.y );
						v.z = M::subtract( this->z, right.z );

						return v;
					}

					typename M::Vector4F
					dot( const Vec3& a )
					{
						return M::add( M::multiply( this->x, a.x ),
									   M::add( M::multiply( this->y, a.y ),
											   M::multiply( this->z, a.z ) ) );
					}

					Vec3
					cross( const Vec3& a )
					{
						Vec3	r;

						r.x = M::subtract( M::multiply( this->y, a.z ), M::multiply( this->z, a.y ) );
						r.y = M::subtract( M::multiply( this->z, a.x ), M::multiply( this->x, a.z ) );
						r.z = M::subtract( M::multiply( this->x, a.y ), M::multiply( this->y, a.x ) );
						
						return r;
					}

					void
					normalize()
					{
						typename M::Vector4F	length = M::sqrtRecip( this->dot( *this ) );
						
						this->x = M::divide( this->x, length );
						this->y = M::divide( this->y, length );
						this->z = M::divide( this->z, length );
					}

					typename M::Vector4F x;
					typename M::Vector4F y;
					typename M::Vector4F z;

				};



			private:

				Vec3	view;
				Vec3	up;
				Vec3	left;
				Vec3	p;
				
			};
			
		}
		
	}

}
