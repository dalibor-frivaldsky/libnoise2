#pragma once




namespace noise
{
	
	namespace proto
	{

		namespace gaborSurface
		{

			template< typename ValueType >
			class Projection
			{

			public:

				Projection( ValueType viewX, ValueType viewY, ValueType viewZ,
							ValueType upX, ValueType upY, ValueType upZ,
							ValueType pX, ValueType pY, ValueType pZ ):
				  view( viewX, viewY, viewZ ),
				  up( upX, upY, upZ ),
				  p( pX, pY, pZ )
				{
					left = view.cross( up );
					left.normalize();

					up = left.cross( view );

					view = -view;
				}

				void
				project( ValueType& projX, ValueType& projY, ValueType& projZ )
				{
					Vec3	tmp( projX, projY, projZ );
					tmp.x = tmp.x - p.x;
					tmp.y = tmp.y - p.y;
					tmp.z = tmp.z - p.z;

					projX = tmp.x * left.x + tmp.y * left.y + tmp.z * left.z;
					projY = tmp.x * up.x + tmp.y * up.y + tmp.z * up.z;
					projZ = tmp.x * view.x + tmp.y * view.y + tmp.z * view.z;
				}



			private:

				struct Vec3
				{

					Vec3():
					  x( ValueType( 0.0 ) ),
					  y( ValueType( 0.0 ) ),
					  z( ValueType( 0.0 ) )
					{}

					Vec3( ValueType x, ValueType y, ValueType z ):
					  x( x ),
					  y( y ),
					  z( z )
					{}

					Vec3
					operator - ()
					{
						Vec3	v;

						v.x = -this->x;
						v.y = -this->y;
						v.z = -this->z;

						return v;
					}

					Vec3
					cross( const Vec3& a )
					{
						Vec3	r;

						r.x = this->y * a.z - this->z * a.y;
						r.y = this->z * a.x - this->x * a.z;
						r.z = this->x * a.y - this->y * a.x;

						return r;
					}

					void
					normalize()
					{
						float	length = std::sqrt( (this->x * this->x) + (this->y * this->y) + (this->z * this->z) );
						
						this->x = this->x / length;
						this->y = this->y / length;
						this->z = this->z / length;
					}

					ValueType x;
					ValueType y;
					ValueType z;

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
