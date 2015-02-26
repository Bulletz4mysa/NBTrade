using System;
using System.ComponentModel;
using System.Xml.Serialization;
using System.Collections;
using System.Drawing;

namespace Easychart.Finance.Objects
{
	public enum SpiralType{Archimedes,Logarithmic,Parabolic,Hyperbolic,Lituus,Golden,GoldenC};
//http://www.horstmann.com/bigj2/labs/Lab18.html
	/// <summary>
	/// Summary description for LogObject.
	/// </summary>
	public class SpiralObject:PolygonObject
	{
		SpiralType spiralType = SpiralType.Archimedes;
		int sweepAngle = 10800;
		double A = 1;
		double B = 0.05;
		//double goldenStartR = 0.01;
		float GOLDEN_MEAN = (float)(1 + Math.Sqrt(5)) / 2; 

		[DefaultValue(SpiralType.Archimedes),XmlAttribute]
		public SpiralType SpiralType
		{
			get
			{
				return spiralType;
			}
			set
			{
				spiralType = value;
			}
		}

		[DefaultValue(1),XmlAttribute]
		public double A_Value
		{
			get
			{
				return A;
			}
			set
			{
				A = value;
			}
		}
		[DefaultValue(0.05),XmlAttribute]
		public double B_Value
		{
			get
			{
				return B;
			}
			set
			{
				B = value;
			}
		}

		[DefaultValue(1800),XmlAttribute]
		public int SweepAngle
		{
			get
			{
				return sweepAngle;
			}
			set
			{
				sweepAngle = value;
			}
		}

		ArrayList alPoint = new ArrayList();
		public SpiralObject()
		{
		}
		
		public void Archimedes()
		{
			spiralType = SpiralType.Archimedes;
		}

		public void Logarithmic()
		{
			spiralType = SpiralType.Logarithmic;
		}
		
		public void Parabolic()
		{
			spiralType = SpiralType.Parabolic;
		}
		
		public void Hyperbolic()
		{
			spiralType = SpiralType.Hyperbolic;
		}
		
		public void Lituus()
		{
			spiralType = SpiralType.Lituus;
		}

		public void Golden()
		{
			spiralType = SpiralType.Golden;
		}

		public void GoldenC()
		{
			spiralType = SpiralType.GoldenC;
		}


		public override PointF[] CalcPoint()
		{
			PointF[] pfs = ToPoints(ControlPoints);
			alPoint.Clear();
			alPoint.Add(pfs[0]);
			float trailingX = pfs[0].X, trailingY=pfs[0].Y; // register for golden cirle center
			float W = pfs[1].X-pfs[0].X;
			float H = pfs[1].Y-pfs[0].Y;
			double StartR = Dist(pfs[0],pfs[1]);
			double StartAngle = Math.Atan2(H,W);
			StartAngle += Math.PI*2;
			Rectangle Rect = Area.Canvas.Rect;
			double Len = Math.Sqrt(Rect.Width*Rect.Width + Rect.Height*Rect.Height);

			//double A = 1;
			switch (spiralType)
			{
				case SpiralType.Archimedes:
					A = StartR/StartAngle;
					break;
				case SpiralType.Logarithmic:
					A = StartR/Math.Exp(StartAngle);
					break;
				case SpiralType.Parabolic:
					A = Math.Sqrt(StartR*StartR/StartAngle);
					break;
				case SpiralType.Hyperbolic:
					A = StartR*StartAngle;
					break;
				case SpiralType.Lituus:
					A = Math.Sqrt(StartR*StartR*StartAngle);
					break;
				case SpiralType.Golden:
					//A = StartR / ((1 + Math.Sqrt(5))/2) ;
				//	if (H > 0 ) A= StartR;
				//	else A = -StartR;
				case SpiralType.GoldenC:
					break;
			}
			float X,Y;
			X = trailingX;//(float)(pfs[0].X+R*Math.Cos(T));
			Y = trailingY;//(float)(pfs[0].Y+R*Math.Sin(T));
			
			for(int i=0, j=0; i<sweepAngle ; i++,j--) 
			{
				double T = (double)i/180*Math.PI+StartAngle;
				double R = 0;
				switch (spiralType)
				{
					case SpiralType.Archimedes:
						R = A*T;
						break;
					case SpiralType.Logarithmic:
						R = A*Math.Exp(B*T);
						break;
					case SpiralType.Parabolic:
						R = Math.Sqrt(A*A*T);
						break; 
					case SpiralType.Hyperbolic:
						R = A/T;
						break;
					case SpiralType.Lituus:
						R = Math.Sqrt(A*A/T);
						break;
					case SpiralType.Golden:		
						R = StartR/Math.Pow (GOLDEN_MEAN,10 );				
//						R = StartR/Math.Pow (GOLDEN_MEAN,510		;
						R *= Math.Pow (GOLDEN_MEAN , Math.Ceiling  ( i / 90f) );
						T = (double)i/180*Math.PI;
						break;
					case SpiralType.GoldenC	:
						R = StartR/Math.Pow (GOLDEN_MEAN,10 );				
						//						R = StartR/Math.Pow (GOLDEN_MEAN,510		;
						R *= Math.Pow (GOLDEN_MEAN , Math.Ceiling  ( i / 90f) );
						T = (double)i/180*Math.PI;
						break;
				}
				if (R>Len) break;				
				if ( spiralType == SpiralType.Golden )
				{
		//			X = (float)(Math.Cos (T) * Math.Pow (GOLDEN_MEAN , ((2 / Math.PI ) * T)));
		//			Y = (float)(Math.Sin (T) * Math.Pow (GOLDEN_MEAN , ((2 / Math.PI ) * T)));
		//		    r^2= x^2+y^2 = phi^(T*4/pi)
					
					if ( i % 360 == 90 )
					{
						trailingY = Y - (float)R * GOLDEN_MEAN;
					}
					else if ( i % 360 == 180 )//91-180
					{						
						trailingX = X + (float)R  * GOLDEN_MEAN;
					}
					else if ( i % 360 == 270 )//181-270
					{
						trailingY = Y + (float)R * GOLDEN_MEAN ;
					}

					else if ( i != 0 && i % 360 == 0 )//271-360
					{
						trailingX = X - (float)R  * GOLDEN_MEAN;
					}
					else
					{
						X = (float)(trailingX+R*Math.Cos(T));
						Y = (float)(trailingY+R*Math.Sin(T));
					}
					

				}
				else if ( spiralType == SpiralType.GoldenC )
				{
					if ( i % 360 == 90 )
					{
						trailingY = Y + (float)R * GOLDEN_MEAN;
					}
					else if ( i % 360 == 180 )//91-180
					{						
						trailingX = X + (float)R  * GOLDEN_MEAN;
					}
					else if ( i % 360 == 270 )//181-270
                    {
						trailingY = Y - (float)R * GOLDEN_MEAN ;
					}

					else if ( i != 0 && i % 360 == 0 )//271-360
					{
						trailingX = X - (float)R  * GOLDEN_MEAN;
					}
					else
					{
						X = (float)(trailingX+R*Math.Cos(T));
						Y = (float)(trailingY-R*Math.Sin(T));
					}

				}
				else // other than golden spiral
				{
 
					X = (float)(pfs[0].X+R*Math.Cos(T));
					Y = (float)(pfs[0].Y+R*Math.Sin(T));
				}
 
				
				if (!float.IsInfinity(X) && !float.IsInfinity(Y))
					alPoint.Add(new PointF(X,Y));
			}
			return (PointF[])alPoint.ToArray(typeof(PointF));
		}

		public override ObjectInit[] RegObject()
		{
			return
				new ObjectInit[]{
				new ObjectInit("Logarithmic Spiral",typeof(SpiralObject),"Logarithmic","Circle","SpiralL",1000),
				new ObjectInit("Archimedes Spiral",typeof(SpiralObject),"Archimedes","Circle","SpiralA"),
				new ObjectInit("Parabolic Spiral",typeof(SpiralObject),"Parabolic","Circle","SpiralP"),
				new ObjectInit("Hyperbolic Spiral",typeof(SpiralObject),"Hyperbolic","Circle","SpiralH"),
				new ObjectInit("Lituus Spiral",typeof(SpiralObject),"Lituus","Circle","SpiralLi"),
				new ObjectInit("Golden Spiral",typeof(SpiralObject),"Golden","Circle","SpiralG"),
				new ObjectInit("Golden CSpiral",typeof(SpiralObject),"GoldenC","Circle","SpiralGC")
				};
		}
	}
}