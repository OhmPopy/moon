//
// Unit tests for FrameworkElementAutomationPeer
//
// Contact:
//   Moonlight List (moonlight-list@lists.ximian.com)
//
// Copyright (C) 2009 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.Collections.Generic;
using System.Reflection;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Automation.Peers;
using System.Windows.Automation.Provider;
using System.Windows.Controls;
using System.Windows.Markup;

using Mono.Moonlight.UnitTesting;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Microsoft.Silverlight.Testing;
using System.Windows.Media;

namespace MoonTest.System.Windows.Automation.Peers {

	[TestClass]
	public class FrameworkElementAutomationPeerTest : SilverlightTest {
		[TestMethod]
		public void CreatePeerForElement ()
		{
			Assert.Throws<ArgumentNullException> (delegate {
				FrameworkElementAutomationPeer.CreatePeerForElement (null);
			}, "null");
		}

		public class ConcreteFrameworkElement : FrameworkElement {
			
			protected override AutomationPeer OnCreateAutomationPeer ()
			{
				return new FrameworkElementAutomationPeerPoker (this);
			}
		}

		[TestMethod]
		public void Null ()
		{
			Assert.Throws<NullReferenceException> (delegate {
				CreateConcreteFrameworkElementAutomationPeer (null);
			});
		}

		[TestMethod]
		public virtual void GetPattern ()
		{
			AutomationPeer feap = FrameworkElementAutomationPeer.CreatePeerForElement (CreateConcreteFrameworkElement ());

			Assert.IsNull (feap.GetPattern (PatternInterface.Dock), "Dock");
			Assert.IsNull (feap.GetPattern (PatternInterface.ExpandCollapse), "ExpandCollapse");
			Assert.IsNull (feap.GetPattern (PatternInterface.Grid), "Grid");
			Assert.IsNull (feap.GetPattern (PatternInterface.GridItem), "GridItem");
			Assert.IsNull (feap.GetPattern (PatternInterface.Invoke), "Invoke");
			Assert.IsNull (feap.GetPattern (PatternInterface.MultipleView), "MultipleView");
			Assert.IsNull (feap.GetPattern (PatternInterface.RangeValue), "RangeValue");
			Assert.IsNull (feap.GetPattern (PatternInterface.Scroll), "Scroll");
			Assert.IsNull (feap.GetPattern (PatternInterface.ScrollItem), "ScrollItem");
			Assert.IsNull (feap.GetPattern (PatternInterface.Selection), "Selection");
			Assert.IsNull (feap.GetPattern (PatternInterface.SelectionItem), "SelectionItem");
			Assert.IsNull (feap.GetPattern (PatternInterface.Table), "Table");
			Assert.IsNull (feap.GetPattern (PatternInterface.TableItem), "TableItem");
			Assert.IsNull (feap.GetPattern (PatternInterface.Toggle), "Toggle");
			Assert.IsNull (feap.GetPattern (PatternInterface.Transform), "Transform");
			Assert.IsNull (feap.GetPattern (PatternInterface.Value), "Value");
			Assert.IsNull (feap.GetPattern (PatternInterface.Window), "Window");
		}

		public interface FrameworkElementAutomationPeerContract {
			AutomationPeer GetLabeledBy ();
			AutomationPeer GetLabeledByCore_ ();
			string GetName ();
			string GetNameCore_ ();
			bool IsContentElement ();
			bool IsContentElementCore_ ();
			bool IsControlElement ();
			bool IsControlElementCore_ ();
			string GetAcceleratorKey ();
			string GetAcceleratorKeyCore_ ();
			string GetAccessKey ();
			string GetAccessKeyCore_ ();
			AutomationControlType GetAutomationControlType ();
			AutomationControlType GetAutomationControlTypeCore_ ();
			string GetAutomationId ();
			string GetAutomationIdCore_ ();
			Rect GetBoundingRectangle ();
			Rect GetBoundingRectangleCore_ ();
			List<AutomationPeer> GetChildren ();
			List<AutomationPeer> GetChildrenCore_ ();
			Point GetClickablePoint ();
			Point GetClickablePointCore_ ();
			string GetHelpText ();
			string GetHelpTextCore_ ();
			string GetItemStatus ();
			string GetItemStatusCore_ ();
			string GetItemType ();
			string GetItemTypeCore_ ();
			string GetLocalizedControlType ();
			string GetLocalizedControlTypeCore_ ();
			AutomationOrientation GetOrientation ();
			AutomationOrientation GetOrientationCore_ ();
			bool HasKeyboardFocus ();
			bool HasKeyboardFocusCore_ ();
			bool IsEnabled ();
			bool IsEnabledCore_ ();
			bool IsKeyboardFocusable ();
			bool IsKeyboardFocusableCore_ ();
			bool IsOffscreen ();
			bool IsOffscreenCore_ ();
			bool IsPassword ();
			bool IsPasswordCore_ ();
			bool IsRequiredForForm ();
			bool IsRequiredForFormCore_ ();
			string GetClassName ();
			string GetClassNameCore_ ();
		}

		public class FrameworkElementAutomationPeerPoker : FrameworkElementAutomationPeer, FrameworkElementAutomationPeerContract {

			public FrameworkElementAutomationPeerPoker (FrameworkElement fe)
				: base (fe)
			{
			}

			public AutomationPeer GetLabeledByCore_ ()
			{
				return base.GetLabeledByCore ();
			}

			public string GetNameCore_ ()
			{
				return base.GetNameCore ();
			}

			public bool IsContentElementCore_ ()
			{
				return base.IsContentElementCore ();
			}

			public bool IsControlElementCore_ ()
			{
				return base.IsControlElementCore ();
			}

			public string GetAcceleratorKeyCore_ ()
			{
				return base.GetAcceleratorKeyCore ();
			}

			public string GetAccessKeyCore_ ()
			{
				return base.GetAccessKeyCore ();
			}

			public AutomationControlType GetAutomationControlTypeCore_ ()
			{
				return base.GetAutomationControlTypeCore ();
			}

			public string GetAutomationIdCore_ ()
			{
				return base.GetAutomationIdCore ();
			}

			public Rect GetBoundingRectangleCore_ ()
			{
				return base.GetBoundingRectangleCore ();
			}

			public List<AutomationPeer> GetChildrenCore_ ()
			{
				return base.GetChildrenCore ();
			}

			public Point GetClickablePointCore_ ()
			{
				return base.GetClickablePointCore ();
			}

			public string GetHelpTextCore_ ()
			{
				return base.GetHelpTextCore ();
			}

			public string GetItemStatusCore_ ()
			{
				return base.GetItemStatusCore ();
			}

			public string GetItemTypeCore_ ()
			{
				return base.GetItemTypeCore ();
			}

			public string GetLocalizedControlTypeCore_ ()
			{
				return base.GetLocalizedControlTypeCore ();
			}

			public AutomationOrientation GetOrientationCore_ ()
			{
				return base.GetOrientationCore ();
			}

			public bool HasKeyboardFocusCore_ ()
			{
				return base.HasKeyboardFocusCore ();
			}

			public bool IsEnabledCore_ ()
			{
				return base.IsEnabledCore ();
			}

			public bool IsKeyboardFocusableCore_ ()
			{
				return base.IsKeyboardFocusableCore ();
			}

			public bool IsOffscreenCore_ ()
			{
				return base.IsOffscreenCore ();
			}

			public bool IsPasswordCore_ ()
			{
				return base.IsPasswordCore ();
			}

			public bool IsRequiredForFormCore_ ()
			{
				return base.IsRequiredForFormCore ();
			}

			public string GetClassNameCore_ ()
			{
				return base.GetClassNameCore ();
			}
		}

		[TestMethod]
		public virtual void GetAcceleratorKey ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetAcceleratorKey (), "GetAcceleratorKey");
			Assert.AreEqual (string.Empty, feap.GetAcceleratorKeyCore_ (), "GetAcceleratorKeyCore");
		}

		[TestMethod]
		public virtual void GetAcceleratorKey_AttachedProperty()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.AreEqual (string.Empty, feap.GetAcceleratorKey (), "GetAcceleratorKey #0");
			Assert.AreEqual (string.Empty, feap.GetAcceleratorKeyCore_ (), "GetAcceleratorKeyCore #0");

			string acceleratorKey = "CTRL+C";

			fe.SetValue (AutomationProperties.AcceleratorKeyProperty, acceleratorKey);
			Assert.AreEqual (acceleratorKey, feap.GetAcceleratorKey (), "GetAcceleratorKey #1");
			Assert.AreEqual (acceleratorKey, feap.GetAcceleratorKeyCore_ (), "GetAcceleratorKeyCore #1");

			fe.SetValue (AutomationProperties.AcceleratorKeyProperty, null);
			Assert.AreEqual (string.Empty, feap.GetAcceleratorKey (), "GetAcceleratorKey #2 YY");
			Assert.AreEqual (string.Empty, feap.GetAcceleratorKeyCore_ (), "GetAcceleratorKeyCore #2 YY");
		}

		[TestMethod]
		public virtual void GetClassName ()
		{
			FrameworkElementAutomationPeerContract feap 
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());

			Assert.AreEqual (string.Empty, feap.GetClassName (), "GetClassNameCore");
			Assert.AreEqual (string.Empty, feap.GetClassNameCore_ (), "GetClassNameCoreCore");
		}

		[TestMethod]
		public virtual void GetAccessKey ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetAccessKey (), "GetAccessKey");
			Assert.AreEqual (string.Empty, feap.GetAccessKeyCore_ (), "GetAccessKeyCore");
		}

		[TestMethod]
		public virtual void GetAccessKey_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.AreEqual (string.Empty, feap.GetAccessKey (), "GetAccessKey");
			Assert.AreEqual (string.Empty, feap.GetAccessKeyCore_ (), "GetAccessKeyCore");

			string accessKey = "ALT+C";

			fe.SetValue (AutomationProperties.AccessKeyProperty, accessKey);
			Assert.AreEqual (accessKey, feap.GetAccessKey (), "GetAccessKey #1");
			Assert.AreEqual (accessKey, feap.GetAccessKeyCore_ (), "GetAccessKeyCore #1");

			fe.SetValue (AutomationProperties.AccessKeyProperty, null);
			Assert.AreEqual (string.Empty, feap.GetAccessKey (), "GetAccessKey #2");
			Assert.AreEqual (string.Empty, feap.GetAccessKeyCore_ (), "GetAccessKeyCore #2");
		}

		[TestMethod]
		public virtual void GetAutomationControlType ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (AutomationControlType.Custom, feap.GetAutomationControlType (), "GetAutomationControlType");
			Assert.AreEqual (AutomationControlType.Custom, feap.GetAutomationControlTypeCore_ (), "GetAutomationControlTypeCore");
		}

		[TestMethod]
		public virtual void GetAutomationId ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetAutomationId (), "GetAutomationId");
			Assert.AreEqual (string.Empty, feap.GetAutomationIdCore_ (), "GetAutomationIdCore");
		}

		[TestMethod]
		public virtual void GetAutomationId_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);
			Assert.AreEqual (string.Empty, feap.GetAutomationId (), "GetAutomationId");
			Assert.AreEqual (string.Empty, feap.GetAutomationIdCore_(), "GetAutomationIdCore");

			string automationId = "MyAttachedAutomationId";

			fe.SetValue(AutomationProperties.AutomationIdProperty, automationId);
			Assert.AreEqual (automationId, feap.GetAutomationId (), "GetAutomationId #1");
			Assert.AreEqual (automationId, feap.GetAutomationIdCore_(), "GetAutomationIdCore #1");

			fe.SetValue (AutomationProperties.AutomationIdProperty, null);
			Assert.AreEqual (string.Empty, feap.GetAutomationId (), "GetAutomationId #2");
			Assert.AreEqual (string.Empty, feap.GetAutomationIdCore_ (), "GetAutomationIdCore #2");
		}

		[TestMethod]
		public virtual void GetBoundingRectangle ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Rect boundingRectangle = feap.GetBoundingRectangle ();
			Assert.AreEqual (0, boundingRectangle.X, "GetBoundingRectangle X");
			Assert.AreEqual (0, boundingRectangle.Y, "GetBoundingRectangle Y");
			Assert.AreEqual (0, boundingRectangle.Width, "GetBoundingRectangle Width");
			Assert.AreEqual (0, boundingRectangle.Height, "GetBoundingRectangle Height");

			boundingRectangle = feap.GetBoundingRectangleCore_ ();
			Assert.AreEqual (0, boundingRectangle.X, "GetBoundingRectangleCore X");
			Assert.AreEqual (0, boundingRectangle.Y, "GetBoundingRectangleCore Y");
			Assert.AreEqual (0, boundingRectangle.Width, "GetBoundingRectangleCore Width");
			Assert.AreEqual (0, boundingRectangle.Height, "GetBoundingRectangleCore Height");

			Assert.AreNotSame (Rect.Empty, boundingRectangle, "GetBoundingRectangleCore Isempty");
		}

		[TestMethod]
		public virtual void GetChildren ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (null, feap.GetChildren (), "GetChildren");
			Assert.AreEqual (null, feap.GetChildrenCore_ (), "GetChildrenCore");
		}

		[TestMethod]
		public virtual void GetClickablePoint ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (0, feap.GetClickablePoint ().X, "GetClickablePoint X");
			Assert.AreEqual (0, feap.GetClickablePoint ().Y, "GetClickablePoint Y");
			Assert.AreEqual (0, feap.GetClickablePointCore_ ().X, "GetClickablePointCore X");
			Assert.AreEqual (0, feap.GetClickablePointCore_ ().Y, "GetClickablePointCore Y");
		}

		[TestMethod]
		public virtual void GetHelpText ()
		{
			FrameworkElementAutomationPeerContract feap 
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetHelpText (), "GetHelpText");
			Assert.AreEqual (string.Empty, feap.GetHelpTextCore_ (), "GetHelpTextCore");
		}

		[TestMethod]
		public virtual void GetHelpText_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);
			Assert.AreEqual (string.Empty, feap.GetHelpText (), "GetHelpText");
			Assert.AreEqual (string.Empty, feap.GetHelpTextCore_ (), "GetHelpTextCore");

			string helpText = "My Help Text property";

			fe.SetValue(AutomationProperties.HelpTextProperty, helpText);
			Assert.AreEqual (helpText, feap.GetHelpText (), "GetHelpText #1");
			Assert.AreEqual (helpText, feap.GetHelpTextCore_ (), "GetHelpTextCore #1");

			fe.SetValue (AutomationProperties.HelpTextProperty, null);
			Assert.AreEqual (string.Empty, feap.GetHelpText (), "GetHelpText #2");
			Assert.AreEqual (string.Empty, feap.GetHelpTextCore_ (), "GetHelpTextCore #2");
		}
		
		[TestMethod]
		public virtual void GetItemStatus ()
		{
			FrameworkElementAutomationPeerContract feap 
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetItemStatus (), "GetItemStatus");
			Assert.AreEqual (string.Empty, feap.GetItemStatusCore_ (), "GetItemStatusCore");
		}

		[TestMethod]
		public virtual void GetItemStatus_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);
			Assert.AreEqual (string.Empty, feap.GetItemStatus (), "GetItemStatus");
			Assert.AreEqual (string.Empty, feap.GetItemStatusCore_ (), "GetItemStatusCore");

			string itemStatus = "My Item Status";

			fe.SetValue (AutomationProperties.ItemStatusProperty, itemStatus);
			Assert.AreEqual (itemStatus, feap.GetItemStatus (), "GetItemStatus #1");
			Assert.AreEqual (itemStatus, feap.GetItemStatusCore_ (), "GetItemStatusCore #1");

			fe.SetValue (AutomationProperties.ItemStatusProperty, null);
			Assert.AreEqual (string.Empty, feap.GetItemStatus (), "GetItemStatus #2");
			Assert.AreEqual (string.Empty, feap.GetItemStatusCore_ (), "GetItemStatusCore #2");
		}

		[TestMethod]
		public virtual void GetItemType ()
		{
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (string.Empty, feap.GetItemType (), "GetItemType");
			Assert.AreEqual (string.Empty, feap.GetItemTypeCore_ (), "GetItemTypeCore");
		}

		[TestMethod]
		public virtual void GetItemType_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.AreEqual (string.Empty, feap.GetItemType (), "GetItemType");
			Assert.AreEqual (string.Empty, feap.GetItemTypeCore_ (), "GetItemTypeCore");

			string itemType = "My Item Type";

			fe.SetValue (AutomationProperties.ItemTypeProperty, itemType);
			Assert.AreEqual (itemType, feap.GetItemType (), "GetItemType #1");
			Assert.AreEqual (itemType, feap.GetItemTypeCore_ (), "GetItemTypeCore #1");

			fe.SetValue (AutomationProperties.ItemTypeProperty, null);
			Assert.AreEqual (string.Empty, feap.GetItemType (), "GetItemType #2");
			Assert.AreEqual (string.Empty, feap.GetItemTypeCore_ (), "GetItemTypeCore #2");
		}

		[TestMethod]
		public virtual void GetLocalizedControlType ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			string localizedString = feap.GetAutomationControlType ().ToString ().ToLower();
			Assert.AreEqual (localizedString, feap.GetLocalizedControlType(), 
					string.Format ("GetLocalizedControlType: {0}", localizedString));
			Assert.AreEqual (localizedString, feap.GetLocalizedControlTypeCore_(), 
					string.Format ("GetLocalizedControlTypeCore: {0}", localizedString));
		}

		[TestMethod]
		public virtual void GetOrientation ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.AreEqual (AutomationOrientation.None, feap.GetOrientation (), "GetOrientation");
			Assert.AreEqual (AutomationOrientation.None, feap.GetOrientationCore_ (), "GetOrientationCore");
		}

		[TestMethod]
		public virtual void HasKeyboardFocus ()
		{
			FrameworkElementAutomationPeerContract feap 
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.IsFalse (feap.HasKeyboardFocus (), "HasKeyboardFocus");
			Assert.IsFalse (feap.HasKeyboardFocusCore_ (), "HasKeyboardFocusCore");
		}

		[TestMethod]
		public virtual void IsKeyboardFocusable ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.IsFalse (feap.IsKeyboardFocusable (), "IsKeyboardFocusable");
			Assert.IsFalse (feap.IsKeyboardFocusableCore_ (), "IsKeyboardFocusableCore");
		}

		[TestMethod]
		public virtual void IsEnabled ()
		{
			FrameworkElementAutomationPeerContract feap
				= CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());

			Assert.IsTrue (feap.IsEnabled (), "IsEnabled");
			Assert.IsTrue (feap.IsEnabledCore_ (), "IsEnabledCore");
		}

		[TestMethod]
		public virtual void IsOffScreen ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerPoker feap = new FrameworkElementAutomationPeerPoker (fe);
			Assert.IsFalse (feap.IsOffscreen (), "IsOffScreen");
			Assert.IsFalse (feap.IsOffscreenCore_ (), "IsOffScreenCore");
		}

		[TestMethod]
		public virtual void IsPassword ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.IsFalse (feap.IsPassword (), "IsPassword");
			Assert.IsFalse (feap.IsPasswordCore_ (), "IsPasswordCore");
		}

		[TestMethod]
		public virtual void IsRequiredForForm ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.IsFalse (feap.IsRequiredForForm (), "IsRequiredForForm");
			Assert.IsFalse (feap.IsRequiredForFormCore_ (), "IsRequiredForFormCore");
		}

		[TestMethod]
		public virtual void IsRequiredForForm_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.IsFalse (feap.IsRequiredForForm (), "IsRequiredForForm");
			Assert.IsFalse (feap.IsRequiredForFormCore_ (), "IsRequiredForFormCore");

			fe.SetValue (AutomationProperties.IsRequiredForFormProperty, true);
			Assert.IsTrue (feap.IsRequiredForForm (), "IsRequiredForForm #1");
			Assert.IsTrue (feap.IsRequiredForFormCore_ (), "IsRequiredForFormCore #1");

			fe.SetValue (AutomationProperties.IsRequiredForFormProperty, false);
			Assert.IsFalse (feap.IsRequiredForForm (), "IsRequiredForForm #2");
			Assert.IsFalse (feap.IsRequiredForFormCore_ (), "IsRequiredForFormCore #2");
		}

		[TestMethod]
		public virtual void GetName ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.AreEqual (String.Empty, feap.GetName (), "GetName");
			Assert.AreEqual (String.Empty, feap.GetNameCore_ (), "GetNameCore");
		}

		[TestMethod]
		public virtual void GetName_AttachedProperty0 ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);

			Assert.AreEqual (string.Empty, feap.GetName (), "GetName");
			Assert.AreEqual (string.Empty, feap.GetNameCore_ (), "GetNameCore");

			string name = "Attached Name";

			fe.SetValue (AutomationProperties.NameProperty, name);
			Assert.AreEqual (name, feap.GetName (), "GetName #1");
			Assert.AreEqual (name, feap.GetNameCore_ (), "GetNameCore #1");

			fe.SetValue (AutomationProperties.NameProperty, null);
			Assert.AreEqual (string.Empty, feap.GetName (), "GetName #2");
			Assert.AreEqual (string.Empty, feap.GetNameCore_ (), "GetNameCore #2");
		}

		[TestMethod]
		public virtual void GetName_AttachedProperty1 ()
		{
			FrameworkElement element = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract tbap = CreateConcreteFrameworkElementAutomationPeer (element);

			string textblockName = "Hello world:";
			string nameProperty = "TextBox name";

			TextBlock textblock = new TextBlock ();
			textblock.Text = textblockName;

			element.SetValue (AutomationProperties.NameProperty, nameProperty);
			Assert.AreEqual (nameProperty, tbap.GetName (), "GetName #0");
			Assert.AreEqual (nameProperty, tbap.GetNameCore_ (), "GetNameCore #0");

			element.SetValue (AutomationProperties.LabeledByProperty, textblock);
			Assert.AreEqual (textblockName, tbap.GetName (), "GetName #1");
			Assert.AreEqual (nameProperty, tbap.GetNameCore_ (), "GetNameCore #1");

			textblock.Text = null;
			Assert.AreEqual (string.Empty, tbap.GetName (), "GetName #2");
			Assert.AreEqual (nameProperty, tbap.GetNameCore_ (), "GetNameCore #2");

			textblock.Text = string.Empty;
			Assert.AreEqual (string.Empty, tbap.GetName (), "GetName #3");
			Assert.AreEqual (nameProperty, tbap.GetNameCore_ (), "GetNameCore #3");

			element.SetValue (AutomationProperties.NameProperty, null);

			Assert.AreEqual (string.Empty, tbap.GetName (), "GetName #4");
			Assert.AreEqual (string.Empty, tbap.GetNameCore_ (), "GetNameCore #4");

			element.SetValue (AutomationProperties.LabeledByProperty, null);

			Assert.AreEqual (string.Empty, tbap.GetName (), "GetName #5");
			Assert.AreEqual (string.Empty, tbap.GetNameCore_ (), "GetNameCore #5");
		}

		[TestMethod]
		public virtual void GetLabeledBy ()
		{
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.IsNull (feap.GetLabeledBy (), "GetLabeledBy");
			Assert.IsNull (feap.GetLabeledByCore_ (), "GetLabeledByCore");
		}

		[TestMethod]
		public virtual void GetLabeledBy_AttachedProperty ()
		{
			FrameworkElement fe = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (fe);
			Assert.IsNull (feap.GetLabeledBy (), "GetLabeledBy");
			Assert.IsNull (feap.GetLabeledByCore_ (), "GetLabeledByCore");

			FrameworkElement labeledBy = new TextBlock ();
			AutomationPeer labeledByPeer = FrameworkElementAutomationPeer.CreatePeerForElement (labeledBy);

			fe.SetValue (AutomationProperties.LabeledByProperty, labeledBy);
			Assert.AreSame (labeledByPeer, feap.GetLabeledBy (), "GetLabeledBy #1");
			Assert.AreSame (labeledByPeer, feap.GetLabeledByCore_(), "GetLabeledByCore #1");

			fe.SetValue (AutomationProperties.LabeledByProperty, null);
			Assert.IsNull (feap.GetLabeledBy (), "GetLabeledBy #2");
			Assert.IsNull (feap.GetLabeledByCore_ (), "GetLabeledByCore #2");
		}

		[TestMethod]
		public virtual void IsContentElement ()
		{
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.IsTrue (feap.IsContentElement (), "IsContentElement");
			Assert.IsTrue (feap.IsContentElementCore_ (), "IsContentElementCore");
		}

		[TestMethod]
		public virtual void IsControlElement ()
		{
			FrameworkElementAutomationPeerContract feap = CreateConcreteFrameworkElementAutomationPeer (CreateConcreteFrameworkElement ());
			Assert.IsTrue (feap.IsControlElement (), "IsControlElement");
			Assert.IsTrue (feap.IsControlElementCore_ (), "IsControlElementCore");
		}
		
		[TestMethod]
		public void CreatePeer ()
		{
			FrameworkElement b = CreateConcreteFrameworkElement ();
			AutomationPeer peer1 = FrameworkElementAutomationPeer.CreatePeerForElement (b);
			AutomationPeer peer2 = FrameworkElementAutomationPeer.CreatePeerForElement (b);
			Assert.IsNotNull (peer1, "#1");
			Assert.AreSame (peer1, peer2, "#2");
		}

		[TestMethod]
		public void CreatePeer2 ()
		{
			FrameworkElement b = CreateConcreteFrameworkElement();
			FrameworkElementAutomationPeer peer1 = new FrameworkElementAutomationPeer (b);
			AutomationPeer peer2 = FrameworkElementAutomationPeer.CreatePeerForElement (b);
			Assert.AreNotSame (peer1, peer2, "#2");
		}
		
		[TestMethod]
		public void CreatePeer3()
		{
			FrameworkElement element = CreateConcreteFrameworkElement();
			FrameworkElementAutomationPeer peer = CreateConcreteFrameworkElementAutomationPeer (element) as FrameworkElementAutomationPeer;
			AutomationPeer realPeer = FrameworkElementAutomationPeer.CreatePeerForElement (element);
			Assert.AreNotSame (peer, realPeer, "#0");

			AutomationPeer anotherRealPeer = FrameworkElementAutomationPeer.CreatePeerForElement(element);
			Assert.AreSame (anotherRealPeer, realPeer, "#1");
		}

		[TestMethod]
		public void FindPeer ()
		{
			Button b = new Button ();
			AutomationPeer peer1 = FrameworkElementAutomationPeer.FromElement (b);
			Assert.IsNull (peer1, "#1");

			peer1 = FrameworkElementAutomationPeer.CreatePeerForElement (b);
			Assert.IsNotNull (peer1, "#2");

			AutomationPeer peer2 = FrameworkElementAutomationPeer.FromElement (b);
			Assert.AreSame (peer1, peer2, "#3");

			peer2 = FrameworkElementAutomationPeer.FromElement (b);
			Assert.AreSame (peer1, peer2, "#4");
		}

		[TestMethod]
		public void Owner ()
		{
			FrameworkElement element = CreateConcreteFrameworkElement ();
			FrameworkElementAutomationPeer realPeer 
				= FrameworkElementAutomationPeer.CreatePeerForElement (element) as FrameworkElementAutomationPeer;
			Assert.AreSame (element, realPeer.Owner, "#0");
		}

		[TestMethod]
		public virtual void ContentTest ()
		{
			Assert.IsFalse (IsContentPropertyElement (), 
				"FrameworkElementAutomationPeer is not ContentElement. Override this method");
		}

		protected bool IsContentPropertyElement ()
		{
			UIElement uielement = CreateConcreteFrameworkElement ();

			object[] attributes
				= uielement.GetType ().GetCustomAttributes (typeof (ContentPropertyAttribute), true);
			if (attributes.Length == 0)
				return false;

			ContentPropertyAttribute attribute = (ContentPropertyAttribute) attributes[0];
			PropertyInfo propertyInfo = uielement.GetType ().GetProperty (attribute.Name,
				BindingFlags.Public | BindingFlags.Instance | BindingFlags.GetProperty);
			if (propertyInfo == null)
				return false;

			MethodInfo methodInfo = propertyInfo.GetGetMethod ();
			if (methodInfo == null)
				return false;

			return true;
		}

		protected virtual FrameworkElement CreateConcreteFrameworkElement ()
		{
			return new ConcreteFrameworkElement ();
		}

		protected virtual FrameworkElementAutomationPeerContract CreateConcreteFrameworkElementAutomationPeer (FrameworkElement element)
		{
			return new FrameworkElementAutomationPeerPoker (element);
		}
	}
}
