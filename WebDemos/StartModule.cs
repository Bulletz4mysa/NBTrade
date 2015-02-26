using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Web;
using System.Data;
using System.IO;
using System.Configuration;
using System.Reflection;
using EasyTools;
using Easychart.Finance.DataClient;
using Easychart.Finance.DataProvider;
using System.Threading;
using Easychart.Finance;

namespace WebDemos
{
	/// <summary>
	/// Summary description for StartModule.
	/// </summary>
	public class StartModule :IHttpModule
	{
		static public int Active;
		static StartModule()
		{
			if (Config.PluginsDirectory==null)
				PluginManager.RegAssemblyFromMemory();
			else PluginManager.Load(Config.PluginsDirectory);

#if(!LITE)
			if (Config.EnableAutoUpdate) 
			{
				Thread t = new Thread(new ThreadStart(AutoUpdate));
				t.Start();
			}

			if (Config.EnableAutoService) 
			{
				Thread t = new Thread(new ThreadStart(AutoService));
				t.Start();
			}

			if (Config.EnableStreaming)
			{
				Thread t = new Thread(new ThreadStart(UpdateStreaming));
				t.Start();
			}
#endif
		}

#if(!LITE)
		static public void SaveHashtable(Hashtable ht,string Filename)
		{
			string s = HttpRuntime.AppDomainAppPath+"Log\\";
			if (!Directory.Exists(s))
				Directory.CreateDirectory(s);
			string FileName = s +Filename; //"Update.log"
			StreamWriter sw = new StreamWriter(FileName);
			foreach(string r in ht.Keys) 
				sw.WriteLine(r+"="+((DateTime)ht[r]).ToString());
			sw.Close();
		}

		static public void LoadHashtable(Hashtable ht,string Filename) 
		{
			try 
			{
				ht.Clear();
				string s = HttpRuntime.AppDomainAppPath+"Log\\";
				string FileName = s +Filename; //"Update.log"
				if (File.Exists(FileName)) 
				{
					StreamReader sr = new StreamReader(FileName);
					try 
					{
						string r;
						while ((r = sr.ReadLine())!=null) 
						{
							int i = r.IndexOf('=');
							if (i>0) 
								ht[r.Substring(0,i)]=DateTime.Parse(r.Substring(i+1));
						}
					} 
					finally 
					{
						sr.Close();
					}
				}
			} 
			catch
			{
			}
		}

		static private void GetTime(string r,out DateTime Time,out DateTime CurrentTime) 
		{
			int TimeDiff = 0;
			int i = r.IndexOf("(");
			int j = r.IndexOf(")",i+1);
			if (i>0 && j>0) 
			{
				TimeDiff = Tools.ToIntDef(r.Substring(i+1,j-i-1),TimeDiff);
				r = r.Remove(i,j-i+1);
			}
			Time = DateTime.Parse(r);
			CurrentTime = DateTime.UtcNow.AddHours(TimeDiff);
		}

		/// <summary>
		/// Update end of day data automatically
		/// </summary>
		public static void AutoUpdate() 
		{
			Thread.Sleep(10000);
			Impersonate.ChangeToAdmin();
			Tools.Log("Auto Update: Started");
			string[] ss = Config.AutoUpdate.Split(',');
			Hashtable ht = new Hashtable();
			while (true) 
				try 
				{
					LoadHashtable(ht,"Update.txt");
					foreach(string s in ss) 
					{
						int i = s.IndexOf("=");
						string Exchange = s.Substring(0,i);
						string r = s.Substring(i+1);

						DateTime LastTime = DateTime.MinValue;
						if (ht[Exchange]!=null)
							LastTime = (DateTime)ht[Exchange];

						DateTime ServiceTime;
						DateTime CurrentTime;
						GetTime(r,out ServiceTime,out CurrentTime);
						
						if (CurrentTime.DayOfWeek!=DayOfWeek.Saturday && CurrentTime.DayOfWeek!=DayOfWeek.Sunday)
						{
							if (CurrentTime.TimeOfDay>ServiceTime.TimeOfDay && LastTime.Date<CurrentTime.Date) 
								try 
								{
									Tools.Log(r+";ServiceTime="+ServiceTime+";CurrentTime="+CurrentTime);
									if (Config.AutoUpdateSource(CurrentTime,Exchange))
									{
										ht[Exchange] = CurrentTime;
										SaveHashtable(ht,"Update.txt");
									}
								}
								catch (Exception ex)
								{
									Tools.Log("AutoUpdate:UpdateQuote:"+ex.Message);
								}
						}
					}
					Thread.Sleep(60000);
				}
				catch (Exception ex)
				{
					Tools.Log("AutoUpdate:Loop:"+ex.Message);
					Thread.Sleep(60000);
				}
		}

		static private void Execute(string ClassName,DateTime d)
		{
			int i=ClassName.LastIndexOf('.');
			if (i>0) 
			{
				string TypeName = ClassName.Substring(0,i);
				ClassName = ClassName.Substring(i+1);
				Type t = Type.GetType(TypeName);
				t.InvokeMember(ClassName,BindingFlags.Static | BindingFlags.Public |  BindingFlags.InvokeMethod,
					null,null,new object[]{"Auto"});
			}
		}

		/// <summary>
		/// Running back ground services at curtain time.
		/// The service and curtain time are defined in web.config
		/// </summary>
		static public void AutoService() 
		{
			Thread.Sleep(1000);
			Impersonate.ChangeToAdmin();
			Tools.Log("Auto Service: Started");
			Hashtable ht = new Hashtable();
			while (true)
				try 
				{
					LoadHashtable(ht,"Service.txt");
					for(int i=1; i<10; i++) 
					{
						string ServiceName = "Service"+i;
						string s = ConfigurationManager.AppSettings[ServiceName];
						if (s!=null)
						{
							string[] ss = s.Split(',');
							if (ss.Length>=2)
							{
								int[] WeekDay = {1,2,3,4,5};
								if (ss.Length==3) 
								{
									string[] sss = ss[2].Split(';');
									WeekDay = new int[sss.Length];
									for(int j=0; j<ss.Length; j++)
										WeekDay[j] = int.Parse(sss[j]);
								}
								DateTime LastTime = DateTime.MinValue;
								if (ht[ServiceName]!=null)
									LastTime = (DateTime)ht[ServiceName];

								DateTime ServiceTime;
								DateTime CurrentTime;
								GetTime(ss[1],out ServiceTime,out CurrentTime);

								if (Array.IndexOf(WeekDay,(int)CurrentTime.DayOfWeek)>=0)
								{
									if (CurrentTime.TimeOfDay>ServiceTime.TimeOfDay && LastTime.Date<CurrentTime.Date)
										try 
										{
											Execute(ss[0],CurrentTime);
											ht[ServiceName] = CurrentTime;
											SaveHashtable(ht,"Service.txt");
										}
										catch (Exception ex)
										{
											Tools.Log(ServiceName+":"+ex.Message);
										}
								}
							}
						}
						Thread.Sleep(1000);
					}
				}
				catch (Exception ex)
				{
					Tools.Log("AutoService:Loop:"+ex.Message);
					Thread.Sleep(10000);
				}
		}

		static DbParam[] dps = new DbParam[]{
												new DbParam("@Symbol",DbType.String,""),
												new DbParam("@QuoteTime",DbType.DateTime,null),
												new DbParam("@Price",DbType.Double,0),
												new DbParam("@Volume",DbType.Double,0),
		};
		static bool StreamingStopped = true;
		/// <summary>
		/// Update streaming data to database
		/// </summary>
		static public void UpdateStreaming()
		{
			Tools.Log("Streaming service started!");
			while (true)
			{
				try
				{
					Thread.Sleep(1000);
					DB.DoCommand("delete from Intraday where QuoteTime<?",
						new DbParam[]{
										 new DbParam("@QuoteTime",DbType.DateTime,DateTime.Today.AddDays(-7))
									 });

					DataClientBase dcb =new EasyChartDataClient();
					dcb.Proxy = Config.WebProxy;
					dcb.OnStreamingData +=new StreamingDataChanged(dcb_OnStreamingData); 
					dcb.OnStreamingStopped+=new EventHandler(dcb_OnStreamingStopped);
					while (true)
					{
						if (StreamingStopped)
						{
							StreamingStopped = false;
							Tools.Log("Streaming service start again!");
							dcb.StartStreaming(Config.IntradaySymbols);
						}
						Thread.Sleep(100);
					}
				} 
				catch (Exception e)
				{
					Tools.Log("Streaming service:"+e);
				}
				Thread.Sleep(1000);
			}
		}

		static private void dcb_OnStreamingStopped(object sender, EventArgs e)
		{
			StreamingStopped = true;
		}

#endif

		#region IHttpModule Members

		public void Init(HttpApplication context)
		{
			// TODO:  Add StartModule.Init implementation
		}

		public void Dispose()
		{
			// TODO:  Add StartModule.Dispose implementation
		}

		#endregion

		private static void dcb_OnStreamingData(object sender, DataPacket dp)
		{
			dps[0].Value = dp.Symbol;
			dps[1].Value = dp.Date;
			dps[2].Value = dp.Close;
			dps[3].Value = dp.Volume;
			DB.DoCommand("insert into Intraday (Symbol,QuoteTime,Price,Volume) values (?,?,?,?)",dps);
		}
	}
}