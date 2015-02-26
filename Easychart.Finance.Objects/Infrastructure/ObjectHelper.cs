using System;

namespace Easychart.Finance.Objects.Infrastructure
{
	/// <summary>
	/// ObjectHelper 的摘要说明。
	/// </summary>
	public class ObjectHelper
	{
		// Methods
		static ObjectHelper()
		{
			ObjectHelper.enUS = new System.Globalization.CultureInfo("en-US");
		}

		public ObjectHelper()
		{
		}

		public static void CreateObjectPath()
		{
			System.IO.Directory.CreateDirectory(ObjectHelper.GetObjectRoot());
		}

		public static string GetIconFile(string IconName)
		{
			if (IconName != null)
			{
				return (ObjectHelper.GetIconRoot() + IconName + ".gif");
			}
			return "";
		}

		public static string GetIconRoot()
		{
			return (ObjectHelper.GetRoot() + @"Icon\");
		}

		public static string GetImageRoot()
		{
			return (ObjectHelper.GetRoot() + @"Images\");
		}

		public static string GetObjectFile(string Symbol)
		{
			return (ObjectHelper.GetObjectRoot() + Symbol + ".xml");
		}

		public static string GetObjectRoot()
		{
			return (ObjectHelper.GetRoot() + @"Object\");
		}

		public static string GetRoot()
		{
			string text1 = System.Reflection.Assembly.GetExecutingAssembly().Location;
			return text1.Substring(0, text1.Length - System.IO.Path.GetFileName(text1).Length);
		}


		// Fields
		public static System.Globalization.CultureInfo enUS;
	}

}
