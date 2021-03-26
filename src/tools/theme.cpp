#include "theme.h"

#ifdef TOOLS_USE_GUI

namespace Tools {

// Draw rectangle
static void drawRectangle(FXDC& dc,FXColor lower,FXColor upper,FXint x,FXint y,FXint w,FXint h){
	register FXint rr,gg,bb,dr,dg,db,r1,g1,b1,r2,g2,b2,yl,yh,yy,dy,n,t;
	const FXint MAXSTEPS=128;
	if(0<w && 0<h){
        dc.setStipple(STIPPLE_NONE);
        dc.setFillStyle(FILL_SOLID);
		
        r1=FXREDVAL(lower);   r2=FXREDVAL(upper);   dr=r2-r1;
        g1=FXGREENVAL(lower); g2=FXGREENVAL(upper); dg=g2-g1;
        b1=FXBLUEVAL(lower);  b2=FXBLUEVAL(upper);  db=b2-b1;

        n=FXABS(dr);
        if((t=FXABS(dg))>n) n=t;
        if((t=FXABS(db))>n) n=t;
        n++;
        if(n>h) n=h;
        if(n>MAXSTEPS) n=MAXSTEPS;
        rr=(r1<<16)+32767;
        gg=(g1<<16)+32767;
        bb=(b1<<16)+32767;
        yy=32767;

        dr=(dr<<16)/n;
        dg=(dg<<16)/n;
        db=(db<<16)/n;
        dy=(h<<16)/n;

        do{
			yl=yy>>16;
			yy+=dy;
			yh=yy>>16;
			dc.setForeground(FXRGB(rr>>16,gg>>16,bb>>16));
			dc.fillRectangle(x,y+yl,w,yh-yl);
			rr+=dr;
			gg+=dg;
			bb+=db;
		}
        while(yh<h);
	}
}

FXDEFMAP(ThemeButton) ThemeButtonMap[]={
	FXMAPFUNC( SEL_PAINT, 0, ThemeButton::onPaint)
};

FXIMPLEMENT( ThemeButton, FXButton, ThemeButtonMap, ARRAYNUMBER(ThemeButtonMap) );


ThemeButton::ThemeButton(FXComposite* p,const FXString& text,FXIcon* ic,FXObject* tgt,FXSelector sel,FXuint opts,
						 FXint x,FXint y,FXint w,FXint h,
						 FXint pl,FXint pr,FXint pt,FXint pb)
	: FXButton( p, text, ic, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb )
{}

// Handle repaint
long ThemeButton::onPaint(FXObject*,FXSelector,void* ptr){
	FXint tw=0,th=0,iw=0,ih=0,tx,ty,ix,iy;
	FXEvent*ev=(FXEvent*)ptr;
	FXDCWindow dc(this,ev);


	FXColor top         = FXRGB(0xfe,0xfd,0xfd);
	FXColor bottom      = FXRGB(0xdd,0xd7,0xce);
	FXColor shade       = FXRGB(0xdc,0xd9,0xd4);

// 	FXColor bordercolor = FXRGB(0x78,0x70,0x63);

	FXColor bordercolor = FXRGB(123,158,189);

/*
  FXPoint borderbackground[12]={FXPoint(0,0),FXPoint(0,1),FXPoint(1,0),
  FXPoint(width-1,0),FXPoint(width-2,0),FXPoint(width-1,1),
  FXPoint(0,height-1),FXPoint(0,height-2),FXPoint(1,height-1),
  FXPoint(width-1,height-1),FXPoint(width-1,height-2),FXPoint(width-2,height-1)};
*/
	FXPoint basebackground[4]={FXPoint(0,0),FXPoint(width-1,0),FXPoint(0,height-1),FXPoint(width-1,height-1)};
                             
	FXPoint bordershade[16]={FXPoint(0,1),FXPoint(1,0),FXPoint(1,2),FXPoint(2,1),
							 FXPoint(width-2,0),FXPoint(width-1,1),FXPoint(width-3,1),FXPoint(width-2,2),
							 FXPoint(0,height-2),FXPoint(1,height-1),FXPoint(1,height-3),FXPoint(2,height-2),
							 FXPoint(width-1,height-2),FXPoint(width-2,height-1),FXPoint(width-2,height-3),FXPoint(width-3,height-2)
	};                               
	FXPoint bordercorners[4]={FXPoint(1,1),FXPoint(1,height-2),FXPoint(width-2,1),FXPoint(width-2,height-2)};


	if (options&BUTTON_TOOLBAR && !underCursor()) {
		dc.setForeground(baseColor);
		dc.fillRectangle(0,0,width,height);
    }
	else if (state==STATE_UP && ((options&BUTTON_TOOLBAR)==0 || (options&BUTTON_TOOLBAR && underCursor()))) {
		/// Outside Background
		dc.setForeground(baseColor);
		dc.drawPoints(basebackground,4);
		/// Border
		dc.setForeground(bordercolor);
		dc.drawRectangle(2,0,width-5,0);
		dc.drawRectangle(2,height-1,width-5,height-1);
		dc.drawRectangle(0,2,0,height-5);
		dc.drawRectangle(width-1,2,0,height-5);
		dc.drawPoints(bordercorners,4);
		dc.setForeground(shade);
		dc.drawPoints(bordershade,16);
		/// Gradient      
		drawRectangle(dc,top,bottom,2,1,width-4,height-2);
		dc.setForeground(top);
		dc.drawRectangle(1,3,0,height-7);
		dc.setForeground(bottom);
		dc.drawRectangle(width-2,3,0,height-7);
    }
	else {
		/// Outside Background
		dc.setForeground(baseColor);
		dc.drawPoints(basebackground,4);
		/// Border
		dc.setForeground(bordercolor);
		dc.drawRectangle(2,0,width-5,0);
		dc.drawRectangle(2,height-1,width-5,height-1);
		dc.drawRectangle(0,2,0,height-5);
		dc.drawRectangle(width-1,2,0,height-5);
		dc.drawPoints(bordercorners,4);
		dc.setForeground(shade);
		dc.drawPoints(bordershade,16);

		dc.setForeground(FXRGB(0xdc,0xd4,0xc9));
		dc.fillRectangle(2,1,width-4,height-2);
    }
 
	// Place text & icon
	if(!label.empty()){
		tw=labelWidth(label);
		th=labelHeight(label);
    }
	if(icon){
		iw=icon->getWidth();
		ih=icon->getHeight();
    }

	just_x(tx,ix,tw,iw);
	just_y(ty,iy,th,ih);

	// Shift a bit when pressed
	if(state && (options&(FRAME_RAISED|FRAME_SUNKEN))){ ++tx; ++ty; ++ix; ++iy; }

	// Draw enabled state
	if(isEnabled()){
		if(icon){
			dc.drawIcon(icon,ix,iy);
		}
		if(!label.empty()){
			dc.setFont(font);
			dc.setForeground(textColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
		if(hasFocus()){
			dc.drawFocusRectangle(border+1,border+1,width-2*border-2,height-2*border-2);
		}
    }

	// Draw grayed-out state
	else{
		if(icon){
			dc.drawIconSunken(icon,ix,iy);
		}
		if(!label.empty()){
			dc.setFont(font);
			dc.setForeground(hiliteColor);
			drawLabel(dc,label,hotoff,tx+1,ty+1,tw,th);
			dc.setForeground(shadowColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
    }
	return 1;
}

FXDEFMAP(ThemeTextField) ThemeTextFieldMap[]={
	FXMAPFUNC( SEL_PAINT, 0, ThemeTextField::onPaint)
};

FXIMPLEMENT( ThemeTextField, FXTextField, ThemeTextFieldMap, ARRAYNUMBER(ThemeTextFieldMap) );

ThemeTextField::ThemeTextField(FXComposite* p,FXint ncols,FXObject* tgt,FXSelector sel,
							   FXuint opts,FXint x,FXint y,FXint w,FXint h,
							   FXint pl,FXint pr,FXint pt,FXint pb)
	: FXTextField( p, ncols, tgt, sel, opts, x, y, w, h, pl, pr, pt, pb )
{
	
}

static void drawFrame(FXDCWindow& dc,FXint x,FXint y,FXint w,FXint h)
{
	if(0<w && 0<h){
		dc.setForeground(FXRGB(123,158,189));
		dc.fillRectangle(x,y,w,1);
		dc.fillRectangle(x,y,1,h);
		// dc.setForeground(hiliteColor);
		dc.fillRectangle(x,y+h-1,w,1);
		dc.fillRectangle(x+w-1,y,1,h);
    }
}


static void drawSunkenFrame(FXDCWindow& dc,FXint x,FXint y,FXint w,FXint h)
{
	if(0<w && 0<h){
		dc.setForeground(FXRGB(255,158,100));
		dc.fillRectangle(x,y,w,1);
		dc.fillRectangle(x,y,1,h);
		// dc.setForeground(hiliteColor);
		dc.fillRectangle(x,y+h-1,w,1);
		dc.fillRectangle(x+w-1,y,1,h);

		if(1<w && 1<h){
			dc.fillRectangle(x+1,y+1,w-3,1);
			dc.fillRectangle(x+1,y+1,1,h-3);
			dc.fillRectangle(x+1,y+h-2,w-2,1);
			dc.fillRectangle(x+w-2,y+1,1,h-2);
		}
    }
}

static void drawHighlightFrame(FXDCWindow& dc,FXint x,FXint y,FXint w,FXint h)
{
	if(0<w && 0<h){
		dc.setForeground(FXRGB(123,158,255));
		dc.fillRectangle(x,y,w,1);
		dc.fillRectangle(x,y,1,h);
		// dc.setForeground(hiliteColor);
		dc.fillRectangle(x,y+h-1,w,1);
		dc.fillRectangle(x+w-1,y,1,h);

		if(1<w && 1<h){
			dc.fillRectangle(x+1,y+1,w-3,1);
			dc.fillRectangle(x+1,y+1,1,h-3);
			dc.fillRectangle(x+1,y+h-2,w-2,1);
			dc.fillRectangle(x+w-2,y+1,1,h-2);
		}
    }
}

long ThemeTextField::onPaint(FXObject*,FXSelector,void* ptr)
{
  FXEvent *ev=(FXEvent*)ptr;
  FXDCWindow dc(this,ev);
  
  // Set font
  dc.setFont(font);
  
  // Draw frame
  Tools::drawFrame(dc,0,0,width,height);
  
  // Gray background if disabled
  dc.setForeground(isEnabled() ? backColor : baseColor);
  
  // Draw background
  dc.fillRectangle(border,border,width-(border<<1),height-(border<<1));
  
  // Draw text, clipped against frame interior
  dc.setClipRectangle(border,border,width-(border<<1),height-(border<<1));
  drawTextRange(dc,0,contents.length());
  
  // Draw caret
  if(flags&FLAG_CARET){
#if FOX_MAJOR <= 1 and FOX_MINOR <= 6
	  int xx=coord(cursor)-1;
	  dc.setForeground(cursorColor);
	  dc.fillRectangle(xx,padtop+border,1,height-padbottom-padtop-(border<<1));
	  dc.fillRectangle(xx-2,padtop+border,5,1);
	  dc.fillRectangle(xx-2,height-border-padbottom-1,5,1);
#else
	  paintCursor(dc);
#endif
  }
  return 1;
}

struct ColorTheme {
  const FXchar* name;
  FXColor base;
  FXColor border;
  FXColor back;
  FXColor fore;
  FXColor selback;
  FXColor selfore;
  FXColor tipback;
  FXColor tipfore;
  FXColor menuback;
  FXColor menufore;
  };


const ColorTheme ColorThemes[]={
	{"Redmond XP",FXRGB(238,238,230),FXRGB(  0,  0,  0),FXRGB(255,255,255),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255),FXRGB(255,255,225),FXRGB(  0,  0,  0),FXRGB( 74,121,205),FXRGB(255,255,255)}
};

void init_theme( FXApp *app )
{
	app->setBaseColor( ColorThemes[0].base );
	app->setBorderColor( ColorThemes[0].border );
	app->setBackColor( ColorThemes[0].back );
	app->setForeColor( ColorThemes[0].fore );
	app->setSelforeColor( ColorThemes[0].selfore );
	app->setSelbackColor( ColorThemes[0].selback );
	app->setTipforeColor( ColorThemes[0].tipfore );
	app->setTipbackColor( ColorThemes[0].tipback );
	app->setSelMenuTextColor( ColorThemes[0].menufore );
	app->setSelMenuBackColor( ColorThemes[0].menuback );
	// app->setHiliteColor(FXRGB(123,158,189));
}

FXDEFMAP(ThemeComboBox) ThemeComboBoxMap[]={
	FXMAPFUNC( SEL_PAINT, 0, ThemeComboBox::onPaint)
};

FXIMPLEMENT( ThemeComboBox, FXComboBox, ThemeComboBoxMap, ARRAYNUMBER(ThemeComboBoxMap) );

ThemeComboBox::ThemeComboBox(FXComposite *p,FXint cols,FXObject* tgt,FXSelector sel,FXuint opts,
							 FXint x,FXint y,FXint w,FXint h,
							 FXint pl,FXint pr,FXint pt,FXint pb)
	: FXComboBox(p,cols, tgt, sel, opts,x,y,w,h, pl,pr,pt,pb)
{
	delete button;

	button=new ThemeMenuButton(this,FXString::null,NULL,pane,FRAME_RAISED|MENUBUTTON_DOWN|MENUBUTTON_ATTACH_RIGHT, 
							0,0,0,0, 0,0,0,0);
	button->setXOffset(border);
	button->setYOffset(border);
}

long ThemeComboBox::onPaint(FXObject*,FXSelector,void* ptr){
	FXEvent *ev=(FXEvent*)ptr;
	FXDCWindow dc(this,ev);
	dc.setForeground(backColor);
	dc.fillRectangle(ev->rect.x,ev->rect.y,ev->rect.w,ev->rect.h);
	Tools::drawFrame(dc,0,0,width,height);
	return 1;
}


FXDEFMAP(ThemeLabel) ThemeLabelMap[]={
	FXMAPFUNC( SEL_PAINT, 0, ThemeLabel::onPaint )
};

FXIMPLEMENT( ThemeLabel, FXLabel, ThemeLabelMap, ARRAYNUMBER( ThemeLabelMap ));

ThemeLabel::ThemeLabel(FXComposite* p,const FXString& text,FXIcon* ic,FXuint opts,
					   FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb)
	: FXLabel( p, text, ic, opts, x, y, w, h, pl, pr, pt, pb )
{
	
}

// Handle repaint
long ThemeLabel::onPaint(FXObject*,FXSelector,void* ptr)
{
	FXEvent   *ev=(FXEvent*)ptr;
	FXDCWindow dc(this,ev);
	FXint      tw=0,th=0,iw=0,ih=0,tx,ty,ix,iy;
	dc.setForeground(backColor);
	dc.fillRectangle(0,0,width,height);
	if(!label.empty()){
		tw=labelWidth(label);
		th=labelHeight(label);
    }
	if(icon){
		iw=icon->getWidth();
		ih=icon->getHeight();
    }
	just_x(tx,ix,tw,iw);
	just_y(ty,iy,th,ih);
	if(icon){
		if(isEnabled())
			dc.drawIcon(icon,ix,iy);
		else
			dc.drawIconSunken(icon,ix,iy);
    }
	if(!label.empty()){
		dc.setFont(font);
		if(isEnabled()){
			dc.setForeground(textColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
		else{
			dc.setForeground(hiliteColor);
			drawLabel(dc,label,hotoff,tx+1,ty+1,tw,th);
			dc.setForeground(shadowColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
    }
	if( options & (FRAME_THICK|FRAME_RAISED|FRAME_SUNKEN) )
	{
		if( options & (FRAME_SUNKEN) )
			Tools::drawSunkenFrame(dc,0,0,width,height);
		else if( options & (FRAME_RAISED) )
			Tools::drawHighlightFrame(dc,0,0,width,height);
		else
			Tools::drawFrame(dc,0,0,width,height);
	}

	return 1;
}

FXIMPLEMENT( ThemeTabItem, FXTabItem, 0, 0);

ThemeTabItem::ThemeTabItem(FXTabBar* p,const FXString& text,FXIcon* ic,FXuint opts,
						   FXint x,FXint y,FXint w,FXint h,FXint pl,FXint pr,FXint pt,FXint pb)
	: FXTabItem(p,text,ic,opts,x,y,w,h,pl,pr,pt,pb)
{
	shadowColor = FXRGB(123,158,189);
	borderColor = FXRGB(123,158,189);
	hiliteColor = FXRGB(123,158,189);
}


ThemeVerticalFrame::ThemeVerticalFrame(FXComposite* p,FXuint opts,FXint x,FXint y,FXint w,FXint h,
									   FXint pl,FXint pr,FXint pt,FXint pb,FXint hs,FXint vs)
	: FXVerticalFrame( p, opts, x, y, w, h, pl, pr, pt, pb, hs, vs )
{
	setHiliteColor( FXRGB(123,158,189) );
}


ThemeHorizontalFrame::ThemeHorizontalFrame(FXComposite* p,FXuint opts,FXint x,FXint y,FXint w,FXint h,
									   FXint pl,FXint pr,FXint pt,FXint pb,FXint hs,FXint vs)
	: FXHorizontalFrame( p, opts, x, y, w, h, pl, pr, pt, pb, hs, vs )
{
	setHiliteColor( FXRGB(123,158,189) );
}

#define MENUBUTTONARROW_WIDTH  13
#define MENUBUTTONARROW_HEIGHT  5

FXDEFMAP(ThemeMenuButton) ThemeMenuButtonMap[]={
	FXMAPFUNC( SEL_PAINT, 0, ThemeMenuButton::onPaint)
};

FXIMPLEMENT( ThemeMenuButton, FXMenuButton, ThemeMenuButtonMap, ARRAYNUMBER(ThemeMenuButtonMap) );


ThemeMenuButton::ThemeMenuButton(FXComposite* p,const FXString& text,
								 FXIcon* ic,FXPopup* pup,FXuint opts,
								 FXint x,FXint y,FXint w,FXint h,
								 FXint pl,FXint pr,FXint pt,FXint pb )
	: FXMenuButton( p, text, ic, pup, opts, x, y, w, h, 
					pl, pr, pt, pb )
{}

// Handle repaint
long ThemeMenuButton::onPaint(FXObject*,FXSelector,void* ptr)
{
	FXint tw=0,th=0,iw=0,ih=0,tx,ty,ix,iy;
	FXEvent *ev=(FXEvent*)ptr;
	FXPoint points[6];
	FXDCWindow dc(this,ev);
/*
	FXColor top         = FXRGB(0xfe,0xfd,0xfd);
	FXColor bottom      = FXRGB(0xdd,0xd7,0xce);
	FXColor shade       = FXRGB(0xdc,0xd9,0xd4);
*/
	FXColor top         = FXRGB(0xde,0xe7,0xff);
	FXColor bottom      = FXRGB(0xbd,0xcf,0xff);
	FXColor shade       = FXRGB(0xbd,0xcb,0xf7);

	FXColor buttoncolor = FXRGB(0x4a,0x61,0x84);

	// FXColor bordercolor = FXRGB(123,158,189);
	FXColor bordercolor = shade;

	FXPoint basebackground[4]={FXPoint(0,0),FXPoint(width-1,0),FXPoint(0,height-1),FXPoint(width-1,height-1)};
                             
	FXPoint bordershade[16]={FXPoint(0,1),FXPoint(1,0),FXPoint(1,2),FXPoint(2,1),
							 FXPoint(width-2,0),FXPoint(width-1,1),FXPoint(width-3,1),FXPoint(width-2,2),
							 FXPoint(0,height-2),FXPoint(1,height-1),FXPoint(1,height-3),FXPoint(2,height-2),
							 FXPoint(width-1,height-2),FXPoint(width-2,height-1),FXPoint(width-2,height-3),FXPoint(width-3,height-2)
	};                               
	FXPoint bordercorners[4]={FXPoint(1,1),FXPoint(1,height-2),FXPoint(width-2,1),FXPoint(width-2,height-2)};


	// Got a border at all?
	if(options&(FRAME_RAISED|FRAME_SUNKEN)){

		// Toolbar style
		if(options&MENUBUTTON_TOOLBAR){

			// Enabled and cursor inside, and not popped up
			if(isEnabled() && underCursor() && !state){
				dc.setForeground(backColor);
				dc.fillRectangle(border,border,width-border*2,height-border*2);
				if(options&FRAME_THICK) drawDoubleRaisedRectangle(dc,0,0,width,height);
				else drawRaisedRectangle(dc,0,0,width,height);
			}

			// Enabled and popped up
			else if(isEnabled() && state){
				dc.setForeground(hiliteColor);
				dc.fillRectangle(border,border,width-border*2,height-border*2);
				if(options&FRAME_THICK) drawDoubleSunkenRectangle(dc,0,0,width,height);
				else drawSunkenRectangle(dc,0,0,width,height);
			}

			// Disabled or unchecked or not under cursor
			else{
				dc.setForeground(backColor);
				dc.fillRectangle(0,0,width,height);
			}
		}

		// Normal style
		else{

			// Draw in up state if disabled or up
			if(!isEnabled() || !state){
				/*
				dc.setForeground(backColor);
				dc.fillRectangle(border,border,width-border*2,height-border*2);
				if(options&FRAME_THICK) drawDoubleRaisedRectangle(dc,0,0,width,height);
				else drawRaisedRectangle(dc,0,0,width,height);
				*/
				/// Outside Background
				dc.setForeground(baseColor);
				dc.drawPoints(basebackground,4);
				/// Border
				dc.setForeground(bordercolor);
				dc.drawRectangle(2,0,width-5,0);
				dc.drawRectangle(2,height-1,width-5,height-1);
				dc.drawRectangle(0,2,0,height-5);
				dc.drawRectangle(width-1,2,0,height-5);
				dc.drawPoints(bordercorners,4);
				dc.setForeground(shade);
				dc.drawPoints(bordershade,16);
				/// Gradient      
				drawRectangle(dc,top,bottom,2,1,width-4,height-2);
				dc.setForeground(top);
				dc.drawRectangle(1,3,0,height-7);
				dc.setForeground(bottom);
				dc.drawRectangle(width-2,3,0,height-7);

			}

			// Draw sunken if enabled and either checked or pressed
			else{
/*
				dc.setForeground(hiliteColor);
				dc.fillRectangle(border,border,width-border*2,height-border*2);
				if(options&FRAME_THICK) drawDoubleSunkenRectangle(dc,0,0,width,height);
				else drawSunkenRectangle(dc,0,0,width,height);
*/
				dc.setForeground(baseColor);
				dc.drawPoints(basebackground,4);
				/// Border
				dc.setForeground(bordercolor);
				dc.drawRectangle(2,0,width-5,0);
				dc.drawRectangle(2,height-1,width-5,height-1);
				dc.drawRectangle(0,2,0,height-5);
				dc.drawRectangle(width-1,2,0,height-5);
				dc.drawPoints(bordercorners,4);
				dc.setForeground(shade);
				dc.drawPoints(bordershade,16);
				
				dc.setForeground(FXRGB(0xdc,0xd4,0xc9));
				dc.fillRectangle(2,1,width-4,height-2);
			}
		}
    }

	// No borders
	else{
		if(isEnabled() && state){
			dc.setForeground(hiliteColor);
			dc.fillRectangle(0,0,width,height);
		}
		else{
			dc.setForeground(backColor);
			dc.fillRectangle(0,0,width,height);
		}
    }

	// Position text & icon
	if(!label.empty()){
		tw=labelWidth(label);
		th=labelHeight(label);
    }

	// Icon?
	if(icon){
		iw=icon->getWidth();
		ih=icon->getHeight();
    }

	// Arrows?
	else if(!(options&MENUBUTTON_NOARROWS)){
		if(options&MENUBUTTON_LEFT){
			ih=MENUBUTTONARROW_WIDTH;
			iw=MENUBUTTONARROW_HEIGHT;
		}
		else{
			iw=MENUBUTTONARROW_WIDTH;
			ih=MENUBUTTONARROW_HEIGHT;
		}
    }

	// Keep some room for the arrow!
	just_x(tx,ix,tw,iw);
	just_y(ty,iy,th,ih);

	// Move a bit when pressed
	if(state){ ++tx; ++ty; ++ix; ++iy; }

	// Draw icon
	if(icon){
		if(isEnabled())
			dc.drawIcon(icon,ix,iy);
		else
			dc.drawIconSunken(icon,ix,iy);
    }

	// Draw arrows
	else if(!(options&MENUBUTTON_NOARROWS)){

		// Right arrow
		if((options&MENUBUTTON_RIGHT)==MENUBUTTON_RIGHT){
			if(isEnabled())
				dc.setForeground(buttoncolor);
			else
				dc.setForeground(shadowColor);
			points[0].x=ix;
			points[0].y=iy;
			points[1].x=ix;
			points[1].y=iy+MENUBUTTONARROW_WIDTH-1;
			points[2].x=ix+MENUBUTTONARROW_HEIGHT;
			points[2].y=(FXshort)(iy+(MENUBUTTONARROW_WIDTH>>1));
			dc.fillPolygon(points,3);
		}

		// Left arrow
		else if(options&MENUBUTTON_LEFT){
			if(isEnabled())
				dc.setForeground(buttoncolor);
			else
				dc.setForeground(shadowColor);
			points[0].x=ix+MENUBUTTONARROW_HEIGHT;
			points[0].y=iy;
			points[1].x=ix+MENUBUTTONARROW_HEIGHT;
			points[1].y=iy+MENUBUTTONARROW_WIDTH-1;
			points[2].x=ix;
			points[2].y=(FXshort)(iy+(MENUBUTTONARROW_WIDTH>>1));
			dc.fillPolygon(points,3);
		}

		// Up arrow
		else if(options&MENUBUTTON_UP){
			if(isEnabled())
				dc.setForeground(buttoncolor);
			else
				dc.setForeground(shadowColor);
			points[0].x=(FXshort)(ix+(MENUBUTTONARROW_WIDTH>>1));
			points[0].y=iy-1;
			points[1].x=ix;
			points[1].y=iy+MENUBUTTONARROW_HEIGHT;
			points[2].x=ix+MENUBUTTONARROW_WIDTH;
			points[2].y=iy+MENUBUTTONARROW_HEIGHT;
			dc.fillPolygon(points,3);
		}

		// Down arrow
		else{
			if(isEnabled())
				dc.setForeground(buttoncolor);
			else
				dc.setForeground(shadowColor);
			points[0].x=ix+1;
			points[0].y=iy;
			points[2].x=ix+MENUBUTTONARROW_WIDTH-1;
			points[2].y=iy;
			points[1].x=(FXshort)(ix+(MENUBUTTONARROW_WIDTH>>1));
			points[1].y=iy+MENUBUTTONARROW_HEIGHT;
			points[3].x=ix+MENUBUTTONARROW_WIDTH-3;
			points[3].y=iy;
			points[4].x=(FXshort)(ix+(MENUBUTTONARROW_WIDTH>>1));
			points[4].y=iy+MENUBUTTONARROW_HEIGHT-3;
			points[5].x=ix+3;
			points[5].y=iy;
			dc.fillConcavePolygon(points,6);
		}
    }

	// Draw text
	if(!label.empty()){
		dc.setFont(font);
		if(isEnabled()){
			dc.setForeground(textColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
		else{
			dc.setForeground(hiliteColor);
			drawLabel(dc,label,hotoff,tx+1,ty+1,tw,th);
			dc.setForeground(shadowColor);
			drawLabel(dc,label,hotoff,tx,ty,tw,th);
		}
    }

	// Draw focus
	if(hasFocus()){
		if(isEnabled()){
			dc.drawFocusRectangle(border+1,border+1,width-2*border-2,height-2*border-2);
		}
    }
	return 1;
}

// Get default width
FXint ThemeMenuButton::getDefaultWidth(){
  FXint tw=0,iw=0,s=0,w,pw;
  if(!label.empty()){ tw=labelWidth(label); s=4; }
  if(!(options&MENUBUTTON_NOARROWS)){
    if(options&MENUBUTTON_LEFT) iw=MENUBUTTONARROW_HEIGHT; else iw=MENUBUTTONARROW_WIDTH;
    }
  if(icon) iw=icon->getWidth();
  if(!(options&(ICON_AFTER_TEXT|ICON_BEFORE_TEXT))) w=FXMAX(tw,iw); else w=tw+iw+s;
  w=padleft+padright+(border<<1)+w;
  if(!(options&MENUBUTTON_LEFT) && (options&MENUBUTTON_ATTACH_RIGHT) && (options&MENUBUTTON_ATTACH_CENTER)){
    if(pane){ pw=pane->getDefaultWidth(); if(pw>w) w=pw; }
    }
  return w;
  }


} // namespace Tools

#endif // TOOLS_USE_GUI
