<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/TR/WD-xsl">
  <xsl:template match="/">
    <HTML>
      <BODY>
        <TABLE style="font:8pt Segoe UI" border="1" CELLSPACING="0" CELLPADDING="1" BGCOLOR="#FFFFFF">
          <THEAD style="font:bold; background-color:#C0C0C0">
            <TR valign="top">
              <xsl:for-each select="/GRID/COLUMNS/COLUMN">
                <TD align="left">
                  <xsl:attribute name="width"><xsl:value-of select="@Width"/></xsl:attribute>
                  <xsl:value-of select="@Caption"/>
                </TD>
              </xsl:for-each>
            </TR>
          </THEAD>
          <xsl:for-each select="GRID/ROWS/ROW">
          <TR valign="top">
            <TD align="left">
              <xsl:value-of select="@Address11"/>
              <xsl:if test="@Address11[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Address21"/>
              <xsl:if test="@Address21[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Address31"/>
              <xsl:if test="@Address31[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Address41"/>
              <xsl:if test="@Address41[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Application_BuildNumber1"/>
              <xsl:if test="@Application_BuildNumber1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ApprovedBy1"/>
              <xsl:if test="@ApprovedBy1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Author1"/>
              <xsl:if test="@Author1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@CheckedBy1"/>
              <xsl:if test="@CheckedBy1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Comment1"/>
              <xsl:if test="@Comment1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@CompanyName1"/>
              <xsl:if test="@CompanyName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Component_Kind1"/>
              <xsl:if test="@Component_Kind1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ComponentKind1"/>
              <xsl:if test="@ComponentKind1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@CurrentDate1"/>
              <xsl:if test="@CurrentDate1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@CurrentTime1"/>
              <xsl:if test="@CurrentTime1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Date1"/>
              <xsl:if test="@Date1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Description1"/>
              <xsl:if test="@Description1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Designator1"/>
              <xsl:if test="@Designator1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Designator_X_Mil_1"/>
              <xsl:if test="@Designator_X_Mil_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Designator_X_mm_1"/>
              <xsl:if test="@Designator_X_mm_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Designator_Y_Mil_1"/>
              <xsl:if test="@Designator_Y_Mil_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Designator_Y_mm_1"/>
              <xsl:if test="@Designator_Y_mm_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@DesignItemId1"/>
              <xsl:if test="@DesignItemId1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Document1"/>
              <xsl:if test="@Document1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@DocumentFullPathAndName1"/>
              <xsl:if test="@DocumentFullPathAndName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@DocumentName1"/>
              <xsl:if test="@DocumentName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@DocumentNumber1"/>
              <xsl:if test="@DocumentNumber1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@DrawnBy1"/>
              <xsl:if test="@DrawnBy1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Engineer1"/>
              <xsl:if test="@Engineer1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Footprint1"/>
              <xsl:if test="@Footprint1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Height_Mil_1"/>
              <xsl:if test="@Height_Mil_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Height_mm_1"/>
              <xsl:if test="@Height_mm_1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Ibis_Model1"/>
              <xsl:if test="@Ibis_Model1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ImagePath1"/>
              <xsl:if test="@ImagePath1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Index1"/>
              <xsl:if test="@Index1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ItemGUID1"/>
              <xsl:if test="@ItemGUID1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@LatestRevisionDate1"/>
              <xsl:if test="@LatestRevisionDate1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@LatestRevisionNote1"/>
              <xsl:if test="@LatestRevisionNote1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Library_Name1"/>
              <xsl:if test="@Library_Name1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Library_Reference1"/>
              <xsl:if test="@Library_Reference1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@LibRef1"/>
              <xsl:if test="@LibRef1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@LogicalDesignator1"/>
              <xsl:if test="@LogicalDesignator1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Model_Footprint1"/>
              <xsl:if test="@Model_Footprint1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Model_Signal_Integrity1"/>
              <xsl:if test="@Model_Signal_Integrity1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Model_Simulation1"/>
              <xsl:if test="@Model_Simulation1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Models1"/>
              <xsl:if test="@Models1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ModifiedDate1"/>
              <xsl:if test="@ModifiedDate1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Organization1"/>
              <xsl:if test="@Organization1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@PackageDescription1"/>
              <xsl:if test="@PackageDescription1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@PackageReference1"/>
              <xsl:if test="@PackageReference1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@PartType1"/>
              <xsl:if test="@PartType1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@PCB3D1"/>
              <xsl:if test="@PCB3D1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@PhysicalPath1"/>
              <xsl:if test="@PhysicalPath1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Pins1"/>
              <xsl:if test="@Pins1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Project1"/>
              <xsl:if test="@Project1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@ProjectName1"/>
              <xsl:if test="@ProjectName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Published1"/>
              <xsl:if test="@Published1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Publisher1"/>
              <xsl:if test="@Publisher1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Quantity1"/>
              <xsl:if test="@Quantity1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Revision1"/>
              <xsl:if test="@Revision1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@RevisionGUID1"/>
              <xsl:if test="@RevisionGUID1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Rule1"/>
              <xsl:if test="@Rule1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@SheetNumber1"/>
              <xsl:if test="@SheetNumber1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@SheetTotal1"/>
              <xsl:if test="@SheetTotal1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Signal_Integrity1"/>
              <xsl:if test="@Signal_Integrity1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Simulation1"/>
              <xsl:if test="@Simulation1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@SourceLibraryName1"/>
              <xsl:if test="@SourceLibraryName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Sub_Parts1"/>
              <xsl:if test="@Sub_Parts1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Time1"/>
              <xsl:if test="@Time1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Title1"/>
              <xsl:if test="@Title1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@UniqueIdName1"/>
              <xsl:if test="@UniqueIdName1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@UniqueIdPath1"/>
              <xsl:if test="@UniqueIdPath1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Value1"/>
              <xsl:if test="@Value1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@Variant1"/>
              <xsl:if test="@Variant1[.='']">&#160;</xsl:if>
            </TD>
            <TD align="left">
              <xsl:value-of select="@VaultGUID1"/>
              <xsl:if test="@VaultGUID1[.='']">&#160;</xsl:if>
            </TD>
          </TR>
          </xsl:for-each>
        </TABLE>
      </BODY>
    </HTML>
  </xsl:template>
</xsl:stylesheet>
