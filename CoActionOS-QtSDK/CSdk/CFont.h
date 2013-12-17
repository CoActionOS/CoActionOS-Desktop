#ifndef CFONT_H
#define CFONT_H

#include <QString>
#include <QFont>
#include <QIcon>
#include <QList>
#include <QFontDatabase>
#include <QPainter>

class CFont
{
public:
  CFont(){}

  static void init(void){
    QFontDatabase::addApplicationFont(":/data/fontawesome-webfont.ttf");
    QFontDatabase::addApplicationFont(":/data/fontawesome-ext-webfont.ttf");
  }

  static QFont font(int size = 24, QString fontName = "FontAwesome"){
    QFontDatabase font;
    return font.font(fontName, "Regular", size);
  }
  static QIcon icon(QString theIcon, int width, int height, QColor color, QString fontName){
    QPixmap pixmap(width, height);
    QPainter p;
    pixmap.fill(Qt::transparent);
    p.begin(&pixmap);
    p.setFont(CFont::font((int)(height*1.25), fontName));
    p.setPen(color);
    p.drawText(pixmap.rect(), theIcon);
    p.end();
    return QIcon(pixmap);
  }

  static void genIconImage(QString name, QString theIcon, int width, int height, QColor color){
    QPixmap pixmap(width, height);
    QPainter p;
    pixmap.fill(Qt::transparent);
    p.begin(&pixmap);
    p.setFont(CFont::font((int)(height)));
    p.setPen(color);
    p.drawText(pixmap.rect(), theIcon);
    p.end();
    pixmap.save(name, "png", -1);
  }

  static inline QString iconApplication(){ return iconBeaker(); }
  static inline QString iconKernel(){ return iconCog(); }
  static inline QString iconPreferences(){ return iconCogs(); }
  static inline QString iconBuild(){ return iconWrench(); }
  static inline QString iconMake(){ return iconBuild(); }
  static inline QString iconConfigure(){ return iconCog(); }
  static inline QString iconInstall(){ return iconDownloadAlt(); }
  static inline QString iconAbout(){ return iconInfoSign(); }
  static inline QString iconUpdate(){ return iconCloud(); }
  static inline QString iconTerminal(){ return iconEdit(); }
  static inline QString iconConsole(){ return iconListAlt(); }
  static inline QString iconMonitor(){ return iconTasks(); }

  static inline QString iconGlass(){ return (QChar(0xf000)); }
  static inline QString iconMusic(){ return (QChar(0xf001)); }
  static inline QString iconSearch(){ return (QChar(0xf002)); }
  static inline QString iconEnvelope(){ return (QChar(0xf003)); }
  static inline QString iconHeart(){ return (QChar(0xf004)); }
  static inline QString iconStar(){ return (QChar(0xf005)); }
  static inline QString iconStarEmpty(){ return (QChar(0xf006)); }
  static inline QString iconUser(){ return (QChar(0xf007)); }
  static inline QString iconFilm(){ return (QChar(0xf008)); }
  static inline QString iconThLarge(){ return (QChar(0xf009)); }
  static inline QString iconTh(){ return (QChar(0xf00a)); }
  static inline QString iconThList(){ return (QChar(0xf00b)); }
  static inline QString iconOk(){ return (QChar(0xf00c)); }
  static inline QString iconRemove(){ return (QChar(0xf00d)); }
  static inline QString iconZoomin(){ return (QChar(0xf00e)); }
  static inline QString iconZoomOut(){ return (QChar(0xf010)); }
  static inline QString iconOff(){ return (QChar(0xf011)); }
  static inline QString iconSignal(){ return (QChar(0xf012)); }
  static inline QString iconCog(){ return (QChar(0xf013)); }
  static inline QString iconTrash(){ return (QChar(0xf014)); }
  static inline QString iconHome(){ return (QChar(0xf015)); }
  static inline QString iconFile(){ return (QChar(0xf016)); }
  static inline QString iconTime(){ return (QChar(0xf017)); }
  static inline QString iconRoad(){ return (QChar(0xf018)); }
  static inline QString iconDownloadAlt(){ return (QChar(0xf019)); }
  static inline QString iconDownload(){ return (QChar(0xf01a)); }
  static inline QString iconUpload(){ return (QChar(0xf01b)); }
  static inline QString iconInbox(){ return (QChar(0xf01c)); }
  static inline QString iconPlayCircle(){ return (QChar(0xf01d)); }
  static inline QString iconRepeat(){ return (QChar(0xf01e)); }
  static inline QString iconRefresh(){ return (QChar(0xf021)); }
  static inline QString iconListAlt(){ return (QChar(0xf022)); }
  static inline QString iconLock(){ return (QChar(0xf023)); }
  static inline QString iconFlag(){ return (QChar(0xf024)); }
  static inline QString iconHeadphones(){ return (QChar(0xf025)); }
  static inline QString iconVolumeOff(){ return (QChar(0xf026)); }
  static inline QString iconVolumeDown(){ return (QChar(0xf027)); }
  static inline QString iconVolumeUp(){ return (QChar(0xf028)); }
  static inline QString iconQrcode(){ return (QChar(0xf029)); }
  static inline QString iconBarcode(){ return (QChar(0xf02a)); }
  static inline QString iconTag(){ return (QChar(0xf02b)); }
  static inline QString iconTags(){ return (QChar(0xf02c)); }
  static inline QString iconBook(){ return (QChar(0xf02d)); }
  static inline QString iconBookmark(){ return (QChar(0xf02e)); }
  static inline QString iconPrint(){ return (QChar(0xf02f)); }
  static inline QString iconCamera(){ return (QChar(0xf030)); }
  static inline QString iconFont(){ return (QChar(0xf031)); }
  static inline QString iconBold(){ return (QChar(0xf032)); }
  static inline QString iconItalic(){ return (QChar(0xf033)); }
  static inline QString iconTextHeight(){ return (QChar(0xf034)); }
  static inline QString iconTextWidth(){ return (QChar(0xf035)); }
  static inline QString iconAlignLeft(){ return (QChar(0xf036)); }
  static inline QString iconAlignCenter(){ return (QChar(0xf037)); }
  static inline QString iconAlignRight(){ return (QChar(0xf038)); }
  static inline QString iconAlignJustify(){ return (QChar(0xf039)); }
  static inline QString iconList(){ return (QChar(0xf03a)); }
  static inline QString iconIndentLeft(){ return (QChar(0xf03b)); }
  static inline QString iconIndentRight(){ return (QChar(0xf03c)); }
  static inline QString iconFacetimeVideo(){ return (QChar(0xf03d)); }
  static inline QString iconPicture(){ return (QChar(0xf03e)); }
  static inline QString iconPencil(){ return (QChar(0xf040)); }
  static inline QString iconMapMarker(){ return (QChar(0xf041)); }
  static inline QString iconAdjust(){ return (QChar(0xf042)); }
  static inline QString iconTint(){ return (QChar(0xf043)); }
  static inline QString iconEdit(){ return (QChar(0xf044)); }
  static inline QString iconShare(){ return (QChar(0xf045)); }
  static inline QString iconCheck(){ return (QChar(0xf046)); }
  static inline QString iconMove(){ return (QChar(0xf047)); }
  static inline QString iconStepBackward(){ return (QChar(0xf048)); }
  static inline QString iconFastBackward(){ return (QChar(0xf049)); }
  static inline QString iconBackward(){ return (QChar(0xf04a)); }
  static inline QString iconPlay(){ return (QChar(0xf04b)); }
  static inline QString iconPause(){ return (QChar(0xf04c)); }
  static inline QString iconStop(){ return (QChar(0xf04d)); }
  static inline QString iconForward(){ return (QChar(0xf04e)); }
  static inline QString iconFastForward(){ return (QChar(0xf050)); }
  static inline QString iconStepForward(){ return (QChar(0xf051)); }
  static inline QString iconEject(){ return (QChar(0xf052)); }
  static inline QString iconChevronLeft(){ return (QChar(0xf053)); }
  static inline QString iconChevronRight(){ return (QChar(0xf054)); }
  static inline QString iconPlusSign(){ return (QChar(0xf055)); }
  static inline QString iconMinusSign(){ return (QChar(0xf056)); }
  static inline QString iconRemoveSign(){ return (QChar(0xf057)); }
  static inline QString iconOkSign(){ return (QChar(0xf058)); }
  static inline QString iconQuestionSign(){ return (QChar(0xf059)); }
  static inline QString iconInfoSign(){ return (QChar(0xf05a)); }
  static inline QString iconScreenshot(){ return (QChar(0xf05b)); }
  static inline QString iconRemoveCircle(){ return (QChar(0xf05c)); }
  static inline QString iconOkCircle(){ return (QChar(0xf05d)); }
  static inline QString iconBanCircle(){ return (QChar(0xf05e)); }
  static inline QString iconArrowLeft(){ return (QChar(0xf060)); }
  static inline QString iconArrowRight(){ return (QChar(0xf061)); }
  static inline QString iconArrowUp(){ return (QChar(0xf062)); }
  static inline QString iconArrowDown(){ return (QChar(0xf063)); }
  static inline QString iconShareAlt(){ return (QChar(0xf064)); }
  static inline QString iconResizeFull(){ return (QChar(0xf065)); }
  static inline QString iconResizeSmall(){ return (QChar(0xf066)); }
  static inline QString iconPlus(){ return (QChar(0xf067)); }
  static inline QString iconMinus(){ return (QChar(0xf068)); }
  static inline QString iconAsterisk(){ return (QChar(0xf069)); }
  static inline QString iconExclamationSign(){ return (QChar(0xf06a)); }
  static inline QString iconGift(){ return (QChar(0xf06b)); }
  static inline QString iconLeaf(){ return (QChar(0xf06c)); }
  static inline QString iconFire(){ return (QChar(0xf06d)); }
  static inline QString iconEyeOpen(){ return (QChar(0xf06e)); }
  static inline QString iconEyeClose(){ return (QChar(0xf070)); }
  static inline QString iconWarningSign(){ return (QChar(0xf071)); }
  static inline QString iconPlane(){ return (QChar(0xf072)); }
  static inline QString iconCalendar(){ return (QChar(0xf073)); }
  static inline QString iconRandom(){ return (QChar(0xf074)); }
  static inline QString iconComment(){ return (QChar(0xf075)); }
  static inline QString iconMagnet(){ return (QChar(0xf076)); }
  static inline QString iconChevronUp(){ return (QChar(0xf077)); }
  static inline QString iconChevronDown(){ return (QChar(0xf078)); }
  static inline QString iconRetweet(){ return (QChar(0xf079)); }
  static inline QString iconShoppingCard(){ return (QChar(0xf07a)); }
  static inline QString iconFolderClose(){ return (QChar(0xf07b)); }
  static inline QString iconFolderOpen(){ return (QChar(0xf07c)); }
  static inline QString iconResizeVertical(){ return (QChar(0xf07d)); }
  static inline QString iconResizeHorizontal(){ return (QChar(0xf07e)); }
  static inline QString iconBarChart(){ return (QChar(0xf080)); }
  static inline QString iconTwitterSign(){ return (QChar(0xf081)); }
  static inline QString iconFacebookSign(){ return (QChar(0xf082)); }
  static inline QString iconCameraRetro(){ return (QChar(0xf083)); }
  static inline QString iconKey(){ return (QChar(0xf084)); }
  static inline QString iconCogs(){ return (QChar(0xf085)); }
  static inline QString iconComments(){ return (QChar(0xf086)); }
  static inline QString iconThumbsUp(){ return (QChar(0xf087)); }
  static inline QString iconThumbsDown(){ return (QChar(0xf088)); }
  static inline QString iconStarHalf(){ return (QChar(0xf089)); }
  static inline QString iconHeartEmpty(){ return (QChar(0xf08a)); }
  static inline QString iconSignout(){ return (QChar(0xf08b)); }
  static inline QString iconLinkedinSign(){ return (QChar(0xf08c)); }
  static inline QString iconPushpin(){ return (QChar(0xf08d)); }
  static inline QString iconExternalLink(){ return (QChar(0xf08e)); }
  static inline QString iconSignin(){ return (QChar(0xf090)); }
  static inline QString iconTrophy(){ return (QChar(0xf091)); }
  static inline QString iconGithubSign(){ return (QChar(0xf092)); }
  static inline QString iconUploadAlt(){ return (QChar(0xf093)); }
  static inline QString iconLemon(){ return (QChar(0xf094)); }
  static inline QString iconPhone(){ return (QChar(0xf095)); }
  static inline QString iconCheckEmpty(){ return (QChar(0xf096)); }
  static inline QString iconBookmarkEmpty(){ return (QChar(0xf097)); }
  static inline QString iconPhoneSign(){ return (QChar(0xf098)); }
  static inline QString iconTwitter(){ return (QChar(0xf099)); }
  static inline QString iconFacebook(){ return (QChar(0xf09a)); }
  static inline QString iconGithub(){ return (QChar(0xf09b)); }
  static inline QString iconUnlock(){ return (QChar(0xf09c)); }
  static inline QString iconCreditCard(){ return (QChar(0xf09d)); }
  static inline QString iconRss(){ return (QChar(0xf09e)); }
  static inline QString iconHdd(){ return (QChar(0xf0a0)); }
  static inline QString iconBullhorn(){ return (QChar(0xf0a1)); }
  static inline QString iconBell(){ return (QChar(0xf0a2)); }
  static inline QString iconCertificate(){ return (QChar(0xf0a3)); }
  static inline QString iconHandRight(){ return (QChar(0xf0a4)); }
  static inline QString iconHandLeft(){ return (QChar(0xf0a5)); }
  static inline QString iconHandUp(){ return (QChar(0xf0a6)); }
  static inline QString iconHandDown(){ return (QChar(0xf0a7)); }
  static inline QString iconCircleArrowLeft(){ return (QChar(0xf0a8)); }
  static inline QString iconCircleArrorRight(){ return (QChar(0xf0a9)); }
  static inline QString iconCircleArrowUp(){ return (QChar(0xf0aa)); }
  static inline QString iconCircleArrowDown(){ return (QChar(0xf0ab)); }
  static inline QString iconGlobe(){ return (QChar(0xf0ac)); }
  static inline QString iconWrench(){ return (QChar(0xf0ad)); }
  static inline QString iconTasks(){ return (QChar(0xf0ae)); }
  static inline QString iconFilter(){ return (QChar(0xf0b0)); }
  static inline QString iconBriefcase(){ return (QChar(0xf0b1)); }
  static inline QString iconFullscreen(){ return (QChar(0xf0b2)); }
  static inline QString iconGroup(){ return (QChar(0xf0c0)); }
  static inline QString iconLink(){ return (QChar(0xf0c1)); }
  static inline QString iconCloud(){ return (QChar(0xf0c2)); }
  static inline QString iconBeaker(){ return (QChar(0xf0c3)); }
  static inline QString iconCut(){ return (QChar(0xf0c4)); }
  static inline QString iconCopy(){ return (QChar(0xf0c5)); }
  static inline QString iconPaperClip(){ return (QChar(0xf0c6)); }
  static inline QString iconSave(){ return (QChar(0xf0c7)); }
  static inline QString iconSignBlank(){ return (QChar(0xf0c8)); }
  static inline QString iconReorder(){ return (QChar(0xf0c9)); }
  static inline QString iconListUl(){ return (QChar(0xf0ca)); }
  static inline QString iconListOl(){ return (QChar(0xf0cb)); }
  static inline QString iconStrikethrough(){ return (QChar(0xf0cc)); }
  static inline QString iconUnderline(){ return (QChar(0xf0cd)); }
  static inline QString iconTable(){ return (QChar(0xf0ce)); }
  static inline QString iconMagic(){ return (QChar(0xf0d0)); }
  static inline QString iconTruck(){ return (QChar(0xf0d1)); }
  static inline QString iconPinterest(){ return (QChar(0xf0d2)); }
  static inline QString iconPinterestSign(){ return (QChar(0xf0d3)); }
  static inline QString iconGooglePlusSign(){ return (QChar(0xf0d4)); }
  static inline QString iconGooglePlus(){ return (QChar(0xf0d5)); }
  static inline QString iconMoney(){ return (QChar(0xf0d6)); }
  static inline QString iconCaretDown(){ return (QChar(0xf0d7)); }
  static inline QString iconCaretUp(){ return (QChar(0xf0d8)); }
  static inline QString iconCaretLeft(){ return (QChar(0xf0d9)); }
  static inline QString iconCaretRight(){ return (QChar(0xf0da)); }
  static inline QString iconColumns(){ return (QChar(0xf0db)); }
  static inline QString iconSort(){ return (QChar(0xf0dc)); }
  static inline QString iconSortDown(){ return (QChar(0xf0dd)); }
  static inline QString iconSortUp(){ return (QChar(0xf0de)); }
  static inline QString iconEnvelopeAlt(){ return (QChar(0xf0e0)); }
  static inline QString iconLinkedin(){ return (QChar(0xf0e1)); }
  static inline QString iconUndo(){ return (QChar(0xf0e2)); }
  static inline QString iconLegal(){ return (QChar(0xf0e3)); }
  static inline QString iconDashboard(){ return (QChar(0xf0e4)); }
  static inline QString iconCommentAlt(){ return (QChar(0xf0e5)); }
  static inline QString iconCommentsAlt(){ return (QChar(0xf0e6)); }
  static inline QString iconBolt(){ return (QChar(0xf0e7)); }
  static inline QString iconSitemap(){ return (QChar(0xf0e8)); }
  static inline QString iconUmbrella(){ return (QChar(0xf0e9)); }
  static inline QString iconPaste(){ return (QChar(0xf0ea)); }
  static inline QString iconlightBulb(){ return (QChar(0xf0eb)); }
  static inline QString iconExchange(){ return QChar(0xf0ec); }
  static inline QString iconCloudDownload(){ return QChar(0xf0ed); }
  static inline QString iconCloudUpload(){ return QChar(0xf0ee); }

  static inline QString iconUserMd(){return QChar(0xf0f0); }
  static inline QString iconStethoscope(){return QChar(0xf0f1); }
  static inline QString iconSuitcase(){return QChar(0xf0f2); }
  static inline QString iconBellAlt(){return QChar(0xf0f3); }
  static inline QString iconCoffee(){return QChar(0xf0f4); }
  static inline QString iconFood(){return QChar(0xf0f5); }
  static inline QString iconFileAlt(){return QChar(0xf0f6); }
  static inline QString iconBuilding(){return QChar(0xf0f7); }
  static inline QString iconHospital(){return QChar(0xf0f8); }
  static inline QString iconAmbulance(){return QChar(0xf0f9); }
  static inline QString iconMedkit(){return QChar(0xf0fa); }
  static inline QString iconFighterJet(){return QChar(0xf0fb); }
  static inline QString iconBeer(){return QChar(0xf0fc); }
  static inline QString iconHSign(){return QChar(0xf0fd); }
  static inline QString iconPlusSignAlt(){return QChar(0xf0fe); }

  static inline QString iconDoubleAngleLeft(){return QChar(0xf100); }
  static inline QString iconDoubleAngleRight(){return QChar(0xf101); }
  static inline QString iconDoubleAngleUp(){return QChar(0xf102); }
  static inline QString iconDoubleAngleDown(){return QChar(0xf103); }
  static inline QString iconAngleLeft(){return QChar(0xf104); }
  static inline QString iconAngleRight(){return QChar(0xf105); }
  static inline QString iconAngleUp(){return QChar(0xf106); }
  static inline QString iconAngleDown(){return QChar(0xf107); }
  static inline QString iconDesktop(){return QChar(0xf108); }
  static inline QString iconLaptop(){return QChar(0xf109); }
  static inline QString iconTablet(){return QChar(0xf10a); }
  static inline QString iconMobilePhone(){return QChar(0xf10b); }
  static inline QString iconCircleBlank(){return QChar(0xf10c); }
  static inline QString iconQuoteLeft(){return QChar(0xf10d); }
  static inline QString iconQuoteRight(){return QChar(0xf10e); }
  static inline QString iconSpinner(){return QChar(0xf110); }
  static inline QString iconCircle(){return QChar(0xf111); }
  static inline QString iconReply(){return QChar(0xf112); }
  static inline QString iconGithubAlt(){return QChar(0xf113); }
  static inline QString iconFolderCloseAlt(){return QChar(0xf114); }
  static inline QString iconFolderOpenAlt(){return QChar(0xf115); }

  //Part of Font-Awesome-More
  static inline QString iconAccessibilitySign(){ return QChar(0xf200); }
  static inline QString iconBikeSign(){ return QChar(0xf201); }
  static inline QString iconBusSign(){ return QChar(0xf202); }
  static inline QString iconCarSign(){ return QChar(0xf203); }
  static inline QString iconTaxiSign(){ return QChar(0xf204); }
  static inline QString iconTruckSign(){ return QChar(0xf205); }
  static inline QString iconAdobePdf(){ return QChar(0xf206); }
  static inline QString iconMsExcel(){ return QChar(0xf207); }
  static inline QString iconMsPpt(){ return QChar(0xf208); }
  static inline QString iconMsWord(){ return QChar(0xf209); }
  static inline QString iconZipFile(){ return QChar(0xf20a); }
  static inline QString iconCss3(){ return QChar(0xf20b); }
  static inline QString iconHtml5(){ return QChar(0xf20c); }
  static inline QString iconLayers(){ return QChar(0xf20d); }
  static inline QString iconMap(){ return QChar(0xf20e); }
  static inline QString iconChrome(){ return QChar(0xf210); }
  static inline QString iconFirefox(){ return QChar(0xf211); }
  static inline QString iconIe(){ return QChar(0xf212); }
  static inline QString iconOpera(){ return QChar(0xf213); }
  static inline QString iconSafari(){ return QChar(0xf214); }
  static inline QString iconRssSign(){ return QChar(0xf215); }

  /*
  static inline QString iconDropbox(){ return QChar(0xf300); }
  static inline QString iconDrupal(){ return QChar(0xf301); }
  static inline QString iconGitFork(){ return QChar(0xf302); }
  static inline QString iconInstagram(){ return QChar(0xf303); }
  static inline QString iconShareThisSign(){ return QChar(0xf304); }
  static inline QString iconShareThis(){ return QChar(0xf305); }
  static inline QString iconFoursquareSign(){ return QChar(0xf306); }
  static inline QString iconFoursquare(){ return QChar(0xf307); }
  static inline QString iconHackerNews(){ return QChar(0xf308); }
  static inline QString iconSkype(){ return QChar(0xf309); }
  static inline QString iconSpotify(){ return QChar(0xf30a); }
  static inline QString iconSoundcloud(){ return QChar(0xf30b); }
  static inline QString iconPaypal(){ return QChar(0xf30c); }
  static inline QString iconYoutubeSign(){ return QChar(0xf30d); }
  static inline QString iconYoutube(){ return QChar(0xf30e); }
  static inline QString iconReddit(){ return QChar(0xf30f); }
  static inline QString iconBloggerSign(){ return QChar(0xf310); }
  static inline QString iconBlogger(){ return QChar(0xf311); }
  static inline QString iconDribbbleSign(){ return QChar(0xf312); }
  static inline QString iconDribbble(){ return QChar(0xf313); }
  static inline QString iconEvernoteSign(){ return QChar(0xf314); }
  static inline QString iconEvernote(){ return QChar(0xf315); }
  static inline QString iconFlickrSign(){ return QChar(0xf316); }
  static inline QString iconFlickr(){ return QChar(0xf317); }
  static inline QString iconForrstSign(){ return QChar(0xf318); }
  static inline QString iconForrst(){ return QChar(0xf319); }
  static inline QString iconDelicious(){ return QChar(0xf31a); }
  static inline QString iconLastfmSign(){ return QChar(0xf31b); }
  static inline QString iconLastfm(){ return QChar(0xf31c); }
  static inline QString iconPicasaSign(){ return QChar(0xf31d); }
  static inline QString iconPicasa(){ return QChar(0xf31e); }
  static inline QString iconStackOverflow(){ return QChar(0xf320); }
  static inline QString iconTumblrSign(){ return QChar(0xf321); }
  static inline QString iconTumblr(){ return QChar(0xf322); }
  static inline QString iconVimeoSign(){ return QChar(0xf323); }
  static inline QString iconVimeo(){ return QChar(0xf324); }
  static inline QString iconWordpressSign(){ return QChar(0xf325); }
  static inline QString iconWordpress(){ return QChar(0xf326); }
  static inline QString iconYelpSign(){ return QChar(0xf327); }
  static inline QString iconYelp(){ return QChar(0xf328); }
  */

  static QString fontAwesome(QString s){ return QString("<font style=\"font-family: FontAwesome\">" + s + "</font>"); }
  static QString icomoon(QString s){ return fontAwesome(s); }
  static QString fontAwesomeMore(QString s){ return QString("<font style=\"font-family: Font-Awesome-More\">" + s + "</font>"); }
};

#endif // CFONT_H
