<?xml version="1.0" encoding="windows-1251"?>
<xsl:stylesheet
     version="1.0"
     xmlns="http://colorer.sf.net/2003/hrc"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output indent="yes"
              cdata-section-elements="regexp start end"
              doctype-public="-//Cail Lomecb//DTD Colorer HRC take5//EN"
              doctype-system="../hrc.dtd" encoding="windows-1251"/>


  <!-- Optional previous colorer.hrc path to easy find type name -->
  <xsl:variable name="root" select="document('file:/d:/programs/devel/colorer.4ever.patch2/hrc/colorer.hrc')"/>

  <!-- New line character -->
  <xsl:template name="crlf">
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <xsl:template match="text()"/>
  <xsl:template match="comment()">
    <xsl:call-template name='crlf'/>
    <xsl:copy/>
  </xsl:template>
  <xsl:template match="@*">
    <xsl:attribute name="{name()}"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>


  <xsl:template match="/hrc">
    <hrc version="take5"
         xmlns="http://colorer.sf.net/2003/hrc"
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://colorer.sf.net/2003/hrc ../hrc.xsd">
      <xsl:choose>
        <xsl:when test="count(./type) > 0">
          <xsl:apply-templates select="type|comment()"/>
        </xsl:when>
        <xsl:when test="count(./type) = 0">
          <type name="{/hrc/scheme[@name = $root/hrc/type/@name]/@name}">
            <xsl:call-template name='crlf'/>
            <import type="default"/>
            <xsl:apply-templates select="include"/>
              <xsl:call-template name='crlf'/>
            <xsl:apply-templates select="define"/>
              <xsl:call-template name='crlf'/>
            <xsl:apply-templates select="entity"/>
              <xsl:call-template name='crlf'/>
            <xsl:apply-templates select="scheme"/>
          </type>
        </xsl:when>
      </xsl:choose>
    </hrc>
  </xsl:template>



  <xsl:template match="type">
    <xsl:element name="prototype">
      <xsl:variable name="group" select="normalize-space(preceding-sibling::comment()[following-sibling::type[1]/@separator or not(preceding-sibling::type)][1])"/>
      <xsl:variable name="sgroup" select="substring-before($group, ' ')"/>
      <xsl:attribute name="name"><xsl:value-of select="@name"/></xsl:attribute>
      <xsl:attribute name="group">
        <xsl:if test="string-length($sgroup) = 0">
          <xsl:value-of select="$group"/>
        </xsl:if><xsl:value-of select="$sgroup"/>
      </xsl:attribute>
      <xsl:attribute name="description"><xsl:value-of select="@descr"/></xsl:attribute>

      <xsl:apply-templates select="load/@name"/>
      <xsl:apply-templates select="@exts"/>
      <xsl:apply-templates select="*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="@exts">
    <filename><xsl:value-of select="."/></filename>
  </xsl:template>

  <xsl:template match="type/@*" priority="-1"/>

  <xsl:template match="load/@name">
    <location link="{.}"/>
  </xsl:template>

  <xsl:template match="switch">
    <xsl:comment>
    Manual change required:
      type: <xsl:value-of select="@type"/>
    &lt;firstline>
      <xsl:value-of select="@match"/>
    &lt;/firstline>
    </xsl:comment>
  </xsl:template>

<!--
  <xsl:template match="params|colors">
    <xsl:element name="ex:{name()}">
      <xsl:apply-templates select="@*"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="colors/@useht">
    <xsl:attribute name="useht">yes</xsl:attribute>
  </xsl:template>

  <xsl:template match="params/@fullback">
    <xsl:attribute name="fullback">yes</xsl:attribute>
  </xsl:template>
-->




  <xsl:template match="include">
    <import>
      <xsl:attribute name="type"><xsl:value-of select="substring-after(substring-before(@name,'.'),'/')"/></xsl:attribute>
    </import>
  </xsl:template>

  <xsl:template match="define">
    <region name="{@name}">
      <xsl:if test="string-length(@value) != 0 and string(number(@value)) = 'NaN'">
        <xsl:attribute name="parent"><xsl:value-of select="@value"/></xsl:attribute>
      </xsl:if>
    </region>
  </xsl:template>

  <xsl:template match="entity">
    <entity name="{@name}" value="{@value}"/>
  </xsl:template>

  <xsl:template match="hrc/scheme">
    <scheme>
      <xsl:apply-templates select="@*|node()"/>
    </scheme>
    <xsl:call-template name='crlf'/>
  </xsl:template>

  <xsl:template match="scheme//*">
    <xsl:element name="{name()}">
      <xsl:apply-templates select="@*|node()"/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="regexp/text() | start/text() | end/text()">
    <xsl:value-of select="."/>
  </xsl:template>

  <xsl:template match="virtual/@subst" priority="2">
    <xsl:attribute name="substScheme"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@lowpriority" priority="2">
    <xsl:attribute name="priority">low</xsl:attribute>
  </xsl:template>

  <xsl:template match="regexp/@region0" priority="2">
    <xsl:attribute name="region"><xsl:value-of select="."/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@worddiv" priority="2">
    <xsl:attribute name="worddiv"><xsl:value-of select="substring-after(substring(.,1,string-length(.)-1),'/')"/></xsl:attribute>
  </xsl:template>

  <xsl:template match="@ignorecase" priority="2">
    <xsl:attribute name="ignorecase">yes</xsl:attribute>
  </xsl:template>

</xsl:stylesheet>
<!-- ***** BEGIN LICENSE BLOCK *****
   - Version: MPL 1.1/GPL 2.0/LGPL 2.1
   -
   - The contents of this file are subject to the Mozilla Public License Version
   - 1.1 (the "License"); you may not use this file except in compliance with
   - the License. You may obtain a copy of the License at
   - http://www.mozilla.org/MPL/
   -
   - Software distributed under the License is distributed on an "AS IS" basis,
   - WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
   - for the specific language governing rights and limitations under the
   - License.
   -
   - The Original Code is the Colorer Library xsd2hrc module.
   -
   - The Initial Developer of the Original Code is
   - Cail Lomecb <cail@nm.ru>.
   - Portions created by the Initial Developer are Copyright (C) 1999-2003
   - the Initial Developer. All Rights Reserved.
   -
   - Contributor(s):
   -
   - Alternatively, the contents of this file may be used under the terms of
   - either the GNU General Public License Version 2 or later (the "GPL"), or
   - the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
   - in which case the provisions of the GPL or the LGPL are applicable instead
   - of those above. If you wish to allow use of your version of this file only
   - under the terms of either the GPL or the LGPL, and not to allow others to
   - use your version of this file under the terms of the MPL, indicate your
   - decision by deleting the provisions above and replace them with the notice
   - and other provisions required by the LGPL or the GPL. If you do not delete
   - the provisions above, a recipient may use your version of this file under
   - the terms of any one of the MPL, the GPL or the LGPL.
   -
   - ***** END LICENSE BLOCK ***** -->
