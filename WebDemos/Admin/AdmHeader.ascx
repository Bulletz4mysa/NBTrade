<%@ Control Language="c#" Inherits="WebDemos.Admin.AdmHeader" CodeBehind="AdmHeader.ascx.cs" %>
<font color="red">Administrator Area </font>: <a class="SiteLink" href="../">Home</a>
| <a class="SiteLink" href="InternetDataToDb.aspx">Internet Data Manager</a> | <a class="SiteLink" href="DataService.aspx">Data Service</a>
| <a class="SiteLink" href="UpdateFormulaValue.aspx"> Update Formula Value</a> | <a class="SiteLink" href="UpdatePreScan.aspx">Update 
	Pre-Scan</a> | <a class="SiteLink" href="AddRemoveData.aspx">Merge Data</a> 
| <a class="SiteLink" href="TestServer.aspx">Test Server</a> | <a class="SiteLink" href="QueryDB.aspx">Query</a> | 
<asp:LinkButton id="lbSignout" runat="server" CssClass="SiteLink" onclick="lbSignout_Click">Sign Out</asp:LinkButton></FONT>
