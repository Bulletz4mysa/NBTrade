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
		private System.Windows.Forms.MenuItem miWebSite;

		public static ObjectDesignerForm MainForm;
		private System.Windows.Forms.Panel pnRight;
		private Easychart.Finance.Objects.ObjectTree ObjectTree;
		private System.Windows.Forms.Splitter spTree;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

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
		static void Main() 
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
			this.ToolPanel = new Easychart.Finance.Objects.ObjectToolPanel();
			this.spTool = new System.Windows.Forms.Splitter();
			this.pg = new System.Windows.Forms.PropertyGrid();
			this.spProperty = new System.Windows.Forms.Splitter();
			this.Designer = new Easychart.Finance.Win.ChartWinControl();
			this.mmMain = new System.Windows.Forms.MainMenu();
			this.miFile = new System.Windows.Forms.MenuItem();
			this.miOpen = new System.Windows.Forms.MenuItem();
			this.miSave = new System.Windows.Forms.MenuItem();
			this.miSp1 = new System.Windows.Forms.MenuItem();
			this.miExit = new System.Windows.Forms.MenuItem();
			this.miHelp = new System.Windows.Forms.MenuItem();
			this.miWebSite = new System.Windows.Forms.MenuItem();
			this.ofd = new System.Windows.Forms.OpenFileDialog();
			this.sfd = new System.Windows.Forms.SaveFileDialog();
			this.pnRight = new System.Windows.Forms.Panel();
			this.spTree = new System.Windows.Forms.Splitter();
			this.ObjectTree = new Easychart.Finance.Objects.ObjectTree();
			this.pnRight.SuspendLayout();
			this.SuspendLayout();
			// 
			// ToolPanel
			// 
			this.ToolPanel.Dock = System.Windows.Forms.DockStyle.Left;
			this.ToolPanel.Location = new System.Drawing.Point(0, 0);
			this.ToolPanel.Name = "ToolPanel";
			this.ToolPanel.ResetAfterEachDraw = true;
			this.ToolPanel.Size = new System.Drawing.Size(140, 461);
			this.ToolPanel.TabIndex = 0;
			// 
			// spTool
			// 
			this.spTool.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.spTool.Location = new System.Drawing.Point(140, 0);
			this.spTool.MinExtra = 80;
			this.spTool.MinSize = 80;
			this.spTool.Name = "spTool";
			this.spTool.Size = new System.Drawing.Size(3, 461);
			this.spTool.TabIndex = 1;
			this.spTool.TabStop = false;
			// 
			// pg
			// 
			this.pg.CommandsVisibleIfAvailable = true;
			this.pg.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pg.LargeButtons = false;
			this.pg.LineColor = System.Drawing.SystemColors.ScrollBar;
			this.pg.Location = new System.Drawing.Point(0, 179);
			this.pg.Name = "pg";
			this.pg.Size = new System.Drawing.Size(200, 282);
			this.pg.TabIndex = 3;
			this.pg.Text = "PropertyGrid";
			this.pg.ViewBackColor = System.Drawing.SystemColors.Window;
			this.pg.ViewForeColor = System.Drawing.SystemColors.WindowText;
			// 
			// spProperty
			// 
			this.spProperty.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.spProperty.Dock = System.Windows.Forms.DockStyle.Right;
			this.spProperty.Location = new System.Drawing.Point(421, 0);
			this.spProperty.MinExtra = 0;
			this.spProperty.MinSize = 0;
			this.spProperty.Name = "spProperty";
			this.spProperty.Size = new System.Drawing.Size(3, 461);
			this.spProperty.TabIndex = 4;
			this.spProperty.TabStop = false;
			// 
			// Designer
			// 
			this.Designer.CausesValidation = false;
			this.Designer.DefaultFormulas = "MAIN#AreaBB#MA(50)#MA(200);VOLMA;SlowSTO;MACD;RSI(14)#RSI(28)";
			this.Designer.Designing = false;
			this.Designer.Dock = System.Windows.Forms.DockStyle.Fill;
			this.Designer.EndTime = new System.DateTime(((long)(0)));
			this.Designer.FavoriteFormulas = "VOLMA;AreaRSI;DEMO.MACD;CCI;OBV;ATR;FastSTO;S" +
				"lowSTO;ROC;TRIX;WR;AD;CMF;PPO;StochRSI;ULT;BBWidth;PVO";
			this.Designer.LatestValueType = Easychart.Finance.LatestValueType.None;
			this.Designer.Location = new System.Drawing.Point(143, 0);
			this.Designer.MaxPrice = 0;
			this.Designer.MinPrice = 0;
			this.Designer.Name = "Designer";
			this.Designer.ShowStatistic = false;
			this.Designer.Size = new System.Drawing.Size(278, 461);
			this.Designer.StartTime = new System.DateTime(((long)(0)));
			this.Designer.Symbol = null;
			this.Designer.TabIndex = 5;
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
			// miSp1
			// 
			this.miSp1.Index = 2;
			this.miSp1.Text = "-";
			// 
			// miExit
			// 
			this.miExit.Index = 3;
			this.miExit.Text = "&Exit";
			this.miExit.Click += new System.EventHandler(this.miExit_Click);
			// 
			// miHelp
			// 
			this.miHelp.Index = 1;
			this.miHelp.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																				   this.miWebSite});
			this.miHelp.Text = "&Help";
			// 
			// miWebSite
			// 
			this.miWebSite.Index = 0;
			this.miWebSite.Text = "&Web Site";
			this.miWebSite.Click += new System.EventHandler(this.miWebSite_Click);
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
			this.pnRight.Location = new System.Drawing.Point(424, 0);
			this.pnRight.Name = "pnRight";
			this.pnRight.Size = new System.Drawing.Size(200, 461);
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
			// ObjectDesignerForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(624, 461);
			this.Controls.Add(this.Designer);
			this.Controls.Add(this.spProperty);
			this.Controls.Add(this.spTool);
			this.Controls.Add(this.ToolPanel);
			this.Controls.Add(this.pnRight);
			this.Font = new System.Drawing.Font("Verdana", 8.25F);
			this.KeyPreview = true;
			this.Menu = this.mmMain;
			this.Name = "ObjectDesignerForm";
			this.Text = "Object Designer Form";
			this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
			this.Load += new System.EventHandler(this.ObjectDesignerForm_Load);
			this.pnRight.ResumeLayout(false);
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
			Process.Start("http://finance.easychart.net");
		}
	}
}