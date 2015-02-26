using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Reflection;
using Easychart.Finance.Objects;
using Easychart.Finance;
using Easychart.Finance.Win;
using Easychart.Finance.DataProvider;
	
namespace StockObjectDemo
{
	/// <summary>
	/// Summary description for ObjectDesignerForm.
	/// </summary>
	public class ObjectDesignerForm : System.Windows.Forms.Form
	{
		private Easychart.Finance.Objects.ObjectToolPanel ToolPanel;
		private System.Windows.Forms.Splitter spTool;
		private System.Windows.Forms.Splitter spProperty;
		private Easychart.Finance.Objects.ObjectManager Manager;
		private System.Windows.Forms.PropertyGrid pg;
        private SizeToolControl sizeToolControl;
		private Easychart.Finance.Win.ChartWinControl Designer;
		private System.Windows.Forms.MainMenu mmMain;
		private System.Windows.Forms.MenuItem miFile;
		private System.Windows.Forms.MenuItem miOpen;
		private System.Windows.Forms.MenuItem miSave;
		private System.Windows.Forms.MenuItem miSp1;
		private System.Windows.Forms.MenuItem miExit;
		private System.Windows.Forms.OpenFileDialog ofd;
		private System.Windows.Forms.SaveFileDialog sfd;
		private System.Windows.Forms.MenuItem miHelp;
		private System.Windows.Forms.MenuItem mAboutMenuItem;

		public static ObjectDesignerForm MainForm;
		private System.Windows.Forms.Panel pnRight;
		private Easychart.Finance.Objects.ObjectTree ObjectTree;
		private System.Windows.Forms.Splitter spTree;
		private System.Windows.Forms.MenuItem mnuSaveImage;
		private System.Windows.Forms.MenuItem mneLoad;
        private System.Windows.Forms.MenuItem submnuYhoo;
        private Panel panel1;
        private IContainer components;

		public ObjectDesignerForm()
		{
			PluginManager.Load(FormulaHelper.Root+"Plugins\\");
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		public static void Main() 
		{
			MainForm = new ObjectDesignerForm();
			Application.Run(MainForm);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            Easychart.Finance.ExchangeIntraday exchangeIntraday1 = new Easychart.Finance.ExchangeIntraday();
            this.sizeToolControl = new Easychart.Finance.Win.SizeToolControl();
            this.Designer = new Easychart.Finance.Win.ChartWinControl();
            this.ToolPanel = new Easychart.Finance.Objects.ObjectToolPanel();
            this.spTool = new System.Windows.Forms.Splitter();
            this.pg = new System.Windows.Forms.PropertyGrid();
            this.spProperty = new System.Windows.Forms.Splitter();
            this.mmMain = new System.Windows.Forms.MainMenu(this.components);
            this.miFile = new System.Windows.Forms.MenuItem();
            this.miOpen = new System.Windows.Forms.MenuItem();
            this.miSave = new System.Windows.Forms.MenuItem();
            this.mneLoad = new System.Windows.Forms.MenuItem();
            this.submnuYhoo = new System.Windows.Forms.MenuItem();
            this.mnuSaveImage = new System.Windows.Forms.MenuItem();
            this.miSp1 = new System.Windows.Forms.MenuItem();
            this.miExit = new System.Windows.Forms.MenuItem();
            this.miHelp = new System.Windows.Forms.MenuItem();
            this.mAboutMenuItem = new System.Windows.Forms.MenuItem();
            this.ofd = new System.Windows.Forms.OpenFileDialog();
            this.sfd = new System.Windows.Forms.SaveFileDialog();
            this.pnRight = new System.Windows.Forms.Panel();
            this.spTree = new System.Windows.Forms.Splitter();
            this.ObjectTree = new Easychart.Finance.Objects.ObjectTree();
            this.panel1 = new System.Windows.Forms.Panel();
            this.pnRight.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // sizeToolControl
            // 
            this.sizeToolControl.ChartControl = this.Designer;
            this.sizeToolControl.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.sizeToolControl.Location = new System.Drawing.Point(0, 466);
            this.sizeToolControl.Name = "sizeToolControl";
            this.sizeToolControl.Size = new System.Drawing.Size(304, 20);
            this.sizeToolControl.TabIndex = 0;
            // 
            // Designer
            // 
            this.Designer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.Designer.CausesValidation = false;
            this.Designer.DefaultFormulas = "MAIN#AreaBB#MA(13)#MA(50)#MA(200);VOLMA;SlowSTO;MACD;RSI(14)#RSI(28);BIAS";
            this.Designer.Designing = false;
		    this.Designer.StickRenderType = StickRenderType.ThickLine;
            this.Designer.EndTime = new System.DateTime(((long)(0)));
            this.Designer.FavoriteFormulas = "VOLMA;AreaRSI;BIAS;MACD;CCI;OBV;ATR;FastSTO;SlowSTO;ROC;TRIX;WR;AD;CMF;PPO;StochR" +
                "SI;ULT;BBWidth;PVO";
            exchangeIntraday1.TimePeriods = new Easychart.Finance.TimePeriod[0];
            exchangeIntraday1.TimeZone = -5;
            this.Designer.IntradayInfo = exchangeIntraday1;
            this.Designer.LatestValueType = Easychart.Finance.LatestValueType.None;
            this.Designer.Location = new System.Drawing.Point(0, 0);
            this.Designer.MaxPrice = 0;
            this.Designer.MinPrice = 0;
            this.Designer.Name = "Designer";
            this.Designer.PriceLabelFormat = null;
            this.Designer.ShowStatistic = false;
            this.Designer.Size = new System.Drawing.Size(304, 460);
            this.Designer.StartTime = new System.DateTime(((long)(0)));
            this.Designer.Symbol = null;
            this.Designer.TabIndex = 5;
            // 
            // ToolPanel
            // 
            this.ToolPanel.Dock = System.Windows.Forms.DockStyle.Left;
            this.ToolPanel.Location = new System.Drawing.Point(3, 0);
            this.ToolPanel.Name = "ToolPanel";
            this.ToolPanel.ResetAfterEachDraw = true;
            this.ToolPanel.Size = new System.Drawing.Size(140, 486);
            this.ToolPanel.TabIndex = 0;
            // 
            // spTool
            // 
            this.spTool.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.spTool.Location = new System.Drawing.Point(0, 0);
            this.spTool.MinExtra = 80;
            this.spTool.MinSize = 80;
            this.spTool.Name = "spTool";
            this.spTool.Size = new System.Drawing.Size(3, 486);
            this.spTool.TabIndex = 1;
            this.spTool.TabStop = false;
            // 
            // pg
            // 
            this.pg.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pg.LineColor = System.Drawing.SystemColors.ScrollBar;
            this.pg.Location = new System.Drawing.Point(0, 179);
            this.pg.Name = "pg";
            this.pg.Size = new System.Drawing.Size(200, 307);
            this.pg.TabIndex = 3;
            // 
            // spProperty
            // 
            this.spProperty.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.spProperty.Dock = System.Windows.Forms.DockStyle.Right;
            this.spProperty.Location = new System.Drawing.Point(447, 0);
            this.spProperty.MinExtra = 0;
            this.spProperty.MinSize = 0;
            this.spProperty.Name = "spProperty";
            this.spProperty.Size = new System.Drawing.Size(3, 486);
            this.spProperty.TabIndex = 4;
            this.spProperty.TabStop = false;
            // 
            // mmMain
            // 
            this.mmMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.miFile,
            this.miHelp});
            // 
            // miFile
            // 
            this.miFile.Index = 0;
            this.miFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.miOpen,
            this.miSave,
            this.mneLoad,
            this.mnuSaveImage,
            this.miSp1,
            this.miExit});
            this.miFile.Text = "&File";
            // 
            // miOpen
            // 
            this.miOpen.Index = 0;
            this.miOpen.Text = "&Open";
            this.miOpen.Click += new System.EventHandler(this.miOpen_Click);
            // 
            // miSave
            // 
            this.miSave.Index = 1;
            this.miSave.Text = "&Save";
            this.miSave.Click += new System.EventHandler(this.miSave_Click);
            // 
            // mneLoad
            // 
            this.mneLoad.Index = 2;
            this.mneLoad.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.submnuYhoo});
            this.mneLoad.Text = "&Load";
            // 
            // submnuYhoo
            // 
            this.submnuYhoo.Index = 0;
            this.submnuYhoo.Text = "Yahoo";
            this.submnuYhoo.Click += new System.EventHandler(this.submnuYhoo_Click);
            // 
            // mnuSaveImage
            // 
            this.mnuSaveImage.Index = 3;
            this.mnuSaveImage.Text = "Save &Image";
            this.mnuSaveImage.Click += new System.EventHandler(this.mnuSaveImage_Click);
            // 
            // miSp1
            // 
            this.miSp1.Index = 4;
            this.miSp1.Text = "-";
            // 
            // miExit
            // 
            this.miExit.Index = 5;
            this.miExit.Text = "&Exit";
            this.miExit.Click += new System.EventHandler(this.miExit_Click);
            // 
            // miHelp
            // 
            this.miHelp.Index = 1;
            this.miHelp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.mAboutMenuItem});
            this.miHelp.Text = "&Help";
            // 
            // mAboutMenuItem
            // 
            this.mAboutMenuItem.Index = 0;
            this.mAboutMenuItem.Text = "&About Adam";
            this.mAboutMenuItem.Click += new System.EventHandler(this.miWebSite_Click);
            // 
            // ofd
            // 
            this.ofd.Filter = "Object File(*.xml)|*.xml";
            // 
            // sfd
            // 
            this.sfd.DefaultExt = "xml";
            this.sfd.Filter = "Object File(*.xml)|*.xml";
            // 
            // pnRight
            // 
            this.pnRight.Controls.Add(this.pg);
            this.pnRight.Controls.Add(this.spTree);
            this.pnRight.Controls.Add(this.ObjectTree);
            this.pnRight.Dock = System.Windows.Forms.DockStyle.Right;
            this.pnRight.Location = new System.Drawing.Point(450, 0);
            this.pnRight.Name = "pnRight";
            this.pnRight.Size = new System.Drawing.Size(200, 486);
            this.pnRight.TabIndex = 6;
            // 
            // spTree
            // 
            this.spTree.Dock = System.Windows.Forms.DockStyle.Top;
            this.spTree.Location = new System.Drawing.Point(0, 176);
            this.spTree.Name = "spTree";
            this.spTree.Size = new System.Drawing.Size(200, 3);
            this.spTree.TabIndex = 5;
            this.spTree.TabStop = false;
            // 
            // ObjectTree
            // 
            this.ObjectTree.Dock = System.Windows.Forms.DockStyle.Top;
            this.ObjectTree.Location = new System.Drawing.Point(0, 0);
            this.ObjectTree.Name = "ObjectTree";
            this.ObjectTree.Size = new System.Drawing.Size(200, 176);
            this.ObjectTree.TabIndex = 4;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.sizeToolControl);
            this.panel1.Controls.Add(this.Designer);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(143, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(304, 486);
            this.panel1.TabIndex = 7;
            // 
            // ObjectDesignerForm
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
            this.ClientSize = new System.Drawing.Size(650, 486);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.ToolPanel);
            this.Controls.Add(this.spProperty);
            this.Controls.Add(this.spTool);
            this.Controls.Add(this.pnRight);
            this.Font = new System.Drawing.Font("Verdana", 8.25F);
            this.KeyPreview = true;
            this.Menu = this.mmMain;
            this.Name = "ObjectDesignerForm";
            this.Text = "Adam Chart Demo";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.Load += new System.EventHandler(this.ObjectDesignerForm_Load);
            this.pnRight.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// Load CSV data from file
		/// </summary>
		/// <param name="FileName"></param>
		private void LoadCSVFile(string Symbol)
		{
			DataManagerBase dmb = new YahooCSVDataManager(Environment.CurrentDirectory,"CSV");
			Designer.DataManager = dmb;
			Designer.Symbol = Symbol;
		}

		private void ObjectDesignerForm_Load(object sender, System.EventArgs e)
		{
				LoadCSVFile("MSFT");
				Manager = new ObjectManager(Designer,pg,ToolPanel,ObjectTree);
				Designer.Skin = "GreenRed";
				Designer.ScaleType = ScaleType.Log;
				KeyMessageFilter.AddMessageFilter(Designer);
		}

		private void miOpen_Click(object sender, System.EventArgs e)
		{
			if (ofd.ShowDialog()==DialogResult.OK)
				Manager.ReadXml(ofd.FileName,true);
		}

		private void miExit_Click(object sender, System.EventArgs e)
		{
			Close();
		}

		private void miSave_Click(object sender, System.EventArgs e)
		{
			if (sfd.ShowDialog()==DialogResult.OK)
				Manager.WriteXml(sfd.FileName);
		}

		private void miWebSite_Click(object sender, System.EventArgs e)
		{
            AboutForm.ShowForm();
		}

		private void mnuSaveImage_Click(object sender, System.EventArgs e)
		{
			Image all  = Manager.GetBitImage();

			System.Windows.Forms.SaveFileDialog dlg = new System.Windows.Forms.SaveFileDialog();
			dlg.Filter = "jpeg files (*.jpg)|*.jpg | gif file (*.gif)|*.gif | bmp file (*.bmp)|*.bmp";			

			if (System.Windows.Forms.DialogResult.OK == dlg.ShowDialog())
			{
				if ( dlg.FilterIndex == 1 )
					all.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Jpeg);
				if ( dlg.FilterIndex == 2 )
					all.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Gif);
				if ( dlg.FilterIndex == 3 )
					all.Save(dlg.FileName, System.Drawing.Imaging.ImageFormat.Bmp);
			}
		}

		private void submnuYhoo_Click(object sender, System.EventArgs e)
		{
			string s = InputBox.ShowInputBox("Quote:","FFIV");
			if (s!=string.Empty ) 
			{
				YahooDataManager ydm = new YahooDataManager();
				ydm.CacheRoot = FormulaHelper.Root+"Cache";
				Designer.DataManager = ydm;
				Designer.EndTime = DateTime.MaxValue;
				Designer.Symbol = s;
			}
		}
	}
}