using System;
using System.Collections;
using System.IO;
using System.Globalization;
using System.Threading;
using Easychart.Finance.DataProvider;

namespace WebDemos
{
	/// <summary>
	/// Summary description for CSVDataManager.
	/// </summary>
	public class CSVDataManager:DataManagerBase
	{
		public CSVDataManager()
		{
		}

		public override IDataProvider GetData(string Code, int Count)
		{
			string FileName = Config.CSVDataPath + Code+Config.CSVExt;
			if (File.Exists(FileName))
			{
				FileStream fs = new FileStream(FileName,FileMode.Open,FileAccess.Read,FileShare.ReadWrite);
				StreamReader sr = new StreamReader(fs);
				try
				{
					string s = sr.ReadToEnd().Trim();
					string[] ss = s.Split('\n');
					ArrayList al = new ArrayList();
					for(int i=0; i<ss.Length; i++) 
					{
						ss[i] = ss[i].Trim();
						if (!ss[i].StartsWith("<!--"))
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
						int j = i;//N-i-1;
						DATE[j] = DateTime.ParseExact(sss[1],"yyyyMMdd",dtfi).ToOADate();
						OPEN[j] = double.Parse(sss[2],nfi);
						HIGH[j] = double.Parse(sss[3],nfi);
						LOW[j] = double.Parse(sss[4],nfi);
						CLOSE[j] = double.Parse(sss[5],nfi);
						VOLUME[j] = double.Parse(sss[6],nfi);
					}

					CommonDataProvider cdp = new CommonDataProvider(this);
					cdp.LoadBinary(new double[][]{OPEN,HIGH,LOW,CLOSE,VOLUME,DATE});
					return cdp;
				}
				finally
				{
					sr.Close();
					fs.Close();
				}
			}
			return base.GetData (Code, Count);
		}
	}
}