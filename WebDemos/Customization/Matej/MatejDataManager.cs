using System;
using System.Collections;
using System.IO;
using System.Globalization;
using System.Threading;
using System.Web;
using Easychart.Finance.DataProvider;

namespace WebDemos
{
	/// <summary>
	/// Summary description for MatejDataManager.
	/// </summary>
	public class MatejDataManager:DataManagerBase
	{
		public MatejDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			string FileName = HttpRuntime.AppDomainAppPath+"data\\"+ Code+".csv";
			if (File.Exists(FileName))
			{
				using (StreamReader sr = File.OpenText(FileName))
				{
					string s = sr.ReadToEnd().Trim();
					string[] ss = s.Split('\n');
					ArrayList al = new ArrayList();
					for(int i=0; i<ss.Length; i++) 
					{
						ss[i] = ss[i].Trim();
						al.Add(ss[i]);
					}
					int N = al.Count;
					double[] CLOSE = new double[N];
					double[] OPEN = new double[N];
					double[] HIGH = new double[N];
					double[] LOW = new double[N];
					double[] VOLUME = new double[N];
					double[] DATE = new double[N];

					DateTimeFormatInfo dtfi = DateTimeFormatInfo.InvariantInfo;
					NumberFormatInfo nfi = NumberFormatInfo.InvariantInfo;
					for (int i=0; i<N; i++) 
					{
						string[] sss = ((string)al[i]).Split(',');
						VOLUME[i] = double.Parse(sss[3],nfi);
						HIGH[i] = double.Parse(sss[4],nfi);
						LOW[i] = double.Parse(sss[5],nfi);
						OPEN[i] = double.Parse(sss[6],nfi);
						CLOSE[i] = double.Parse(sss[7],nfi);
						string r = sss[8];
						int j = r.IndexOf('T');
						if (j>0)
							r = r.Substring(1,j-1);

						DATE[i] = DateTime.ParseExact(r,"yyyy-MM-dd",dtfi).ToOADate();
					}

					CommonDataProvider cdp = new CommonDataProvider(this);
					cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
					return cdp;

				}
			}
			return base.GetData (Code, Count);
		}

	}
}
