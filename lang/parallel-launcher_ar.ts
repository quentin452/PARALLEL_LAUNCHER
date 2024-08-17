<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ar">
<context>
    <name>AbstractCoreFinderDialog</name>
    <message>
        <location filename="../src/ui/core-finder-dialog.cpp" line="10"/>
        <source>Searching for latest core version...</source>
        <translatorcomment>I decided to use the same word that Retroarch uses for &quot;Core&quot;.</translatorcomment>
        <translation>يجري البحث عن أحدث إصدار من النواه ...</translation>
    </message>
</context>
<context>
    <name>BindInputDialog</name>
    <message>
        <location filename="../src/ui/designer/bind-input-dialog.ui" line="14"/>
        <source>Bind Input</source>
        <translatorcomment>Can also be ربط أجهزة الإدخال which is &quot;bind device input&quot;. Not sure about character limits with rendering and all that, which is why I went for the short version.</translatorcomment>
        <translation>ربط الإدخال</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/bind-input-dialog.ui" line="38"/>
        <source>To map a button or axis to this input, either press a button on your controller or hold a control stick or trigger in a different position for a half second.</source>
        <translatorcomment>&quot;Controller&quot; is a PITA. You can use عصا التحكم which also means &quot;joystick&quot; or &quot;control stick&quot; (it works for both controller and the actual sticks themselves), or you can use &quot;وحدة التحكم&quot;  which also means &quot;control unit&quot; or &quot; ذراع التحكم&quot; which is what I used because it&apos;s the most common but also translates to &quot;control arm&quot;.</translatorcomment>
        <translation>لتعيين زر أو محور لهذا الإدخال ، إما أن تضغط على زر في ذراع التحكم الخاصة بك أو اضغط على عصا التحكم أو الزنأد في وضع مختلف لمدة نصف ثانية.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/bind-input-dialog.ui" line="76"/>
        <source>Skip</source>
        <translation>تخطى</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/bind-input-dialog.ui" line="87"/>
        <source>Cancel</source>
        <translation>إلغاء</translation>
    </message>
</context>
<context>
    <name>ControllerConfigDialog</name>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="14"/>
        <source>Configure Controller</source>
        <translatorcomment>Went with closest to retroarch. Can also be عدل ذراع التحكم or  أضبط ذراع التحكم or  ضبط ذراع التحكم</translatorcomment>
        <translation>ضبط ذراع التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="25"/>
        <source>Configuring Controller:</source>
        <translatorcomment>I don&apos;t like this one, will come back to it.</translatorcomment>
        <translation>تطبيق إعدادات ذراع التحكم:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="62"/>
        <source>Quick Configure</source>
        <translatorcomment>This also translates to &quot;quick setup&quot;.</translatorcomment>
        <translation>ضبط سريع</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="86"/>
        <source>Enable Rumble</source>
        <translatorcomment>1:1 translates to &quot;enable vibration&quot; but follows what microsoft uses. Can also be تفعيل الدمدمة  or تفعيل القعقعة</translatorcomment>
        <translation>تمكين الاهتزاز</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="99"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="19"/>
        <source>Analog Up</source>
        <translatorcomment>can also be عصا التناظرية أعلى like retroarch&apos;s translation, but I don&apos;t like Retroarch&apos;s translation.</translatorcomment>
        <translation>عصا التناظرية فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="122"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="20"/>
        <source>Analog Down</source>
        <translatorcomment>can also be عصا التناظرية أسفل like Retroarch but I don&apos;t like Retroarch&apos;s translation.</translatorcomment>
        <translation>عصا التناظرية تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="145"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="21"/>
        <source>Analog Left</source>
        <translation>عصا التناظرية يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="168"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="22"/>
        <source>Analog Right</source>
        <translation>عصا التناظرية يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="191"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="31"/>
        <source>A Button</source>
        <translatorcomment>So if phonetically translating, it would be زر أي  , if trying for a 1:1 it would be زر أ  , I feel keeping the button letters in English like the actual controller is a better idea so that&apos;s what I did.</translatorcomment>
        <translation>زر A</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="214"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="32"/>
        <source>B Button</source>
        <translatorcomment>Same as in Button A. If you don&apos;t want the english letters left, let me know and I&apos;ll update these.</translatorcomment>
        <translation>زر B</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="248"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="23"/>
        <source>C Up</source>
        <translatorcomment>I added the word &quot;button&quot; to get the damn thing to render properly since it doesn&apos;t like the mixture of RTL with LTR when starting with English. If you can figure it out (C needs to be to the right of the wordفوق) , remove زر</translatorcomment>
        <translation>زر C فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="271"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="24"/>
        <source>C Down</source>
        <translatorcomment>same as C up</translatorcomment>
        <translation>زر C  تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="294"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="25"/>
        <source>C Left</source>
        <translatorcomment>Same as C Up</translatorcomment>
        <translation>زر C  يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="317"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="26"/>
        <source>C Right</source>
        <translatorcomment>Same as C Up</translatorcomment>
        <translation>زر C  يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="340"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="34"/>
        <source>Z Trigger</source>
        <translation>زناد Z</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="363"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="35"/>
        <source>R Trigger</source>
        <translation>زناد R</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="397"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="27"/>
        <source>D-Pad Up</source>
        <translation>زر الاتجاهات فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="420"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="28"/>
        <source>D-Pad Down</source>
        <translation>زر الاتجاهات تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="443"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="29"/>
        <source>D-Pad Left</source>
        <translation>زر الاتجاهات يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="466"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="30"/>
        <source>D-Pad Right</source>
        <translation>زر الاتجاهات يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="489"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="36"/>
        <source>Start Button</source>
        <translatorcomment>If you don&apos;t translate the word (which lots of arab gamers don&apos;t) you would use زر ستارت  instead, where start is transliterated. but what I put in is considered &quot;proper&quot; and is what retroarch uses</translatorcomment>
        <translation>زر البدء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="512"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="33"/>
        <source>L Trigger</source>
        <translation>زناد L</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="546"/>
        <source>The following inputs are only used by games that support Gamecube Controllers:</source>
        <translation>المدخلات التالية مستخدمة فقط في الألعاب التي تدعم أذرعات تحكم Gamecube:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="566"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="37"/>
        <source>X Button</source>
        <translation>زر X</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="573"/>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="597"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="75"/>
        <source>Not Bound</source>
        <translation>غير ملزمة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="610"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="38"/>
        <source>Y Button</source>
        <translation>زر Y</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="660"/>
        <source>Sensitivity</source>
        <translation>حساسية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="686"/>
        <source>Deadzone</source>
        <translation>المنطقة الميتة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="745"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="39"/>
        <source>Save State</source>
        <translation>حفظ إلحالة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="774"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="40"/>
        <source>Load State</source>
        <translation>تحميل إلحالة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="822"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="42"/>
        <source>Toggle Slow Motion</source>
        <translation>تبديل &quot;الحركة البطيئة&quot;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="832"/>
        <location filename="../src/ui/controller-config-dialog.cpp" line="41"/>
        <source>Toggle Fast Forward</source>
        <translation>تبديل التقديم السريع</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="864"/>
        <source>How far you need to press down a trigger or tilt a control stick that is bound to an N64 button before it is considered a button press</source>
        <translation>إلى أي مدى تحتاج إلى الضغط على الزناد أو إمالة عصا التحكم المرتبطة بزر N64 قبل اعتبارها ضغطة على الزر</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="867"/>
        <source>Analog to Digital Button Press Threshold</source>
        <translation>عتبة ضغط تحويل الزر التناظري إلى رقمي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="949"/>
        <source>Save As</source>
        <translation>احفظ باسم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="969"/>
        <source>Save</source>
        <translation>احفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-config-dialog.ui" line="983"/>
        <source>Cancel</source>
        <translation>إلغاء</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="78"/>
        <source>Button %1</source>
        <translation>زر %1</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="81"/>
        <source>Axis -%1</source>
        <translation>محور -%1</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="84"/>
        <source>Axis +%1</source>
        <translation>محور +%1</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="87"/>
        <source>Hat %1 Up</source>
        <translation>لوحة تحريك بالإبهام %1 فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="90"/>
        <source>Hat %1 Down</source>
        <translation>لوحة تحريك بالإبهام %1 تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="93"/>
        <source>Hat %1 Left</source>
        <translation>لوحة تحريك بالإبهام %1 يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="96"/>
        <source>Hat %1 Right</source>
        <translation>لوحة تحريك بالإبهام %1 يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="141"/>
        <source>Enter Profile Name</source>
        <translation>أدخل اسم ملف الشخصي</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="141"/>
        <source>Enter a name for your new controller profile.</source>
        <translation>أدخل اسمًا لملف تعريف ذراع التحكم الجديد الخاص بك.</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="147"/>
        <source>Failed to Save Profile</source>
        <translation>فشل حفظ الملف الشخصي</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-config-dialog.cpp" line="147"/>
        <source>A default controller profile with that name already exists.</source>
        <translation>ملف تعريف وحدة تحكم افتراضي بهذا الاسم موجود بالفعل.</translation>
    </message>
</context>
<context>
    <name>ControllerSelectDialog</name>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="14"/>
        <source>Select Controller</source>
        <translation>إختر ذراع التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="28"/>
        <source>Input Driver</source>
        <translation>مشغل الإدخال</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="81"/>
        <source>If your controller does not appear in this list, or if you are having problems setting up your controller bindings, try switching to a different input driver.</source>
        <translation>إذا لم تظهر ذراع التحكم الخاصة بك في هذه القائمة ، أو إذا كنت تواجه مشاكل في إعداد روابط ذراع التحكم الخاصة بك ، فحاول التبديل إلى برنامج تشغيل إدخال مختلف.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="125"/>
        <source>Connected Controllers</source>
        <translation>أذرعة التحكم المتصلة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="154"/>
        <source>Controller Profiles</source>
        <translation>ملف ذراع التحكم الشخصي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="169"/>
        <location filename="../src/ui/controller-select-dialog.cpp" line="178"/>
        <location filename="../src/ui/controller-select-dialog.cpp" line="220"/>
        <source>Edit Profile</source>
        <translation>حرر الملف الشخصي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/controller-select-dialog.ui" line="180"/>
        <source>Delete Profile</source>
        <translation>احذف الملف الشخصي</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-select-dialog.cpp" line="28"/>
        <source>Could not get connected controllers because a file (&lt;tt&gt;parallel-launcher-sdl-relay.exe&lt;/tt&gt;) is missing from your Parallel Launcher installation. Please re-install Parallel Launcher to restore the missing file(s). If this problem persists, you may need to whitelist the &lt;a href=&quot;#&quot;&gt;Parallel Launcher installation directory&lt;/a&gt; in your anti-virus software.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/ui/controller-select-dialog.cpp" line="35"/>
        <source>Input driver failed to initialize due to an unexpected error.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/ui/controller-select-dialog.cpp" line="174"/>
        <location filename="../src/ui/controller-select-dialog.cpp" line="216"/>
        <source>New Profile</source>
        <translation>ملف شخصي جديد</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-select-dialog.cpp" line="269"/>
        <source>Confirm Delete</source>
        <translation>أكد الحذف</translation>
    </message>
    <message>
        <location filename="../src/ui/controller-select-dialog.cpp" line="269"/>
        <source>Are you sure you want to delete controller profile &apos;%1&apos;?</source>
        <translation>هل تريد بالتأكيد حذف ملف تعريف ذراع التحكم &apos;%1&apos;؟</translation>
    </message>
</context>
<context>
    <name>CoreInstaller</name>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="26"/>
        <location filename="../src/ui/core-installer.cpp" line="77"/>
        <source>Download Failed</source>
        <translation>فشل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="27"/>
        <location filename="../src/ui/core-installer.cpp" line="78"/>
        <source>Failed to download emulator core</source>
        <translation>فشل تنزيل نواة المحاكي</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="35"/>
        <location filename="../src/ui/core-installer.cpp" line="110"/>
        <source>Installation Failed</source>
        <translation>فشل التثبيت</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="36"/>
        <location filename="../src/ui/core-installer.cpp" line="111"/>
        <source>Failed to install emulator core</source>
        <translation>فشل تثبيت نواة المحاكي</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="46"/>
        <location filename="../src/ui/core-installer.cpp" line="122"/>
        <source>Installation Successful</source>
        <translation>نجح التثبيت</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="47"/>
        <location filename="../src/ui/core-installer.cpp" line="123"/>
        <source>Core installed successfully.</source>
        <translation>نجح تثبيت النوة.</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="150"/>
        <source>Core Unavailable</source>
        <translation>النواة غير متوفرة</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="151"/>
        <source>Sorry! The Mupen64plus-Next emulator core is not available on MacOS. Try using ParallelN64 instead.</source>
        <translation>آسف! نواة المحاكي Mupen64plus-Next غير متوفرة على نظام MacOS. حاول استخدام ParallelN64 بدلا من ذلك.</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="159"/>
        <source>Install Emulator Core?</source>
        <translation>تثبيت نواة المحاكي؟</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="160"/>
        <source>This emulator core is not installed or is out of date. Would you like to install it now?</source>
        <translation>نواة المحاكي غير مثبت أو أنه قديم الطراز. هل ترغب في تثبيته الان؟</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="214"/>
        <location filename="../src/ui/core-installer.cpp" line="245"/>
        <source>Core Update Available</source>
        <translation>يوجد تحديث للنواة</translation>
    </message>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="215"/>
        <location filename="../src/ui/core-installer.cpp" line="246"/>
        <source>An update is available for %1. Would you like to install it now?</source>
        <translation>يوجد تحديث لـ%1. هل ترغب في تثبيته الان؟</translation>
    </message>
</context>
<context>
    <name>CrashReportDialog</name>
    <message>
        <location filename="../src/ui/designer/crash-report-dialog.ui" line="14"/>
        <source>View Crash Reports</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/crash-report-dialog.ui" line="26"/>
        <source>Only show crashes from the current application version</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/crash-report-dialog.ui" line="61"/>
        <source>Crash Report</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DeviceBusyDialog</name>
    <message>
        <location filename="../src/ui/designer/device-busy-dialog.ui" line="17"/>
        <source>Device Busy</source>
        <translation>الجهاز مشغول</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/device-busy-dialog.ui" line="26"/>
        <source>One or more SD cards are busy. Waiting for all file operations to complete...</source>
        <translation>واحدة أو أكثر من بطاقات الSD مشغولة. في انتظار اكتمال كافة عمليات الملف...</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/device-busy-dialog.ui" line="58"/>
        <source>Force Unmount</source>
        <translatorcomment>note if the context is say &quot;force unmount SD card&quot; the translation would be فرض إلغاء تحميل بطاقة sd</translatorcomment>
        <translation>فرض إلغاء تحميل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/device-busy-dialog.ui" line="69"/>
        <source>Cancel</source>
        <translation>إلغاء</translation>
    </message>
</context>
<context>
    <name>DirectPlay</name>
    <message>
        <location filename="../src/ui/play.cpp" line="677"/>
        <location filename="../src/ui/play.cpp" line="684"/>
        <location filename="../src/ui/play.cpp" line="691"/>
        <location filename="../src/ui/play.cpp" line="698"/>
        <source>Patch Failed</source>
        <translation>فشل الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="678"/>
        <source>Failed to patch ROM. The .bps patch appears to be invalid.</source>
        <translation>فشل ترقيع إلROM. يبدو أن ترقيع إلbps غير صالح.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="685"/>
        <source>Failed to patch ROM. The base ROM does not match what the patch expects..</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي لا يتطابق مع ما يتوقعه الترقيع ..</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="692"/>
        <source>Failed to patch ROM. The base rom required to patch is not known to Parallel Launcher.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي المطلوب للترقيعه غير معروف لـ Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="699"/>
        <source>Failed to patch ROM. An error occurred while writing the patched ROM to disk.</source>
        <translation>فشل ترقيع إلROM. حدث خطأ أثناء كتابة إلROM المرقعه إلى  القرص.</translation>
    </message>
</context>
<context>
    <name>DirectPlayWindow</name>
    <message>
        <location filename="../src/ui/designer/direct-play-window.ui" line="14"/>
        <source>ROM Settings</source>
        <translatorcomment>transliterated ROM instead of translated as done by major arab romhacking/gaming  sites like pixelarab.com</translatorcomment>
        <translation>إعدادات إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/direct-play-window.ui" line="27"/>
        <source>This is your first time running this ROM. Please select any ROM specific settings. You can always change these settings later.</source>
        <translation>هذه هي المرة الأولى التي تقوم فيها بتشغيل هذا إلROM. الرجاء تحديد أي إعدادات محددة للROM . يمكنك دائمًا تغيير هذه الإعدادات لاحقًا.</translation>
    </message>
</context>
<context>
    <name>DownloadDialog</name>
    <message>
        <location filename="../src/ui/core-installer.cpp" line="22"/>
        <location filename="../src/ui/core-installer.cpp" line="73"/>
        <source>Downloading core...</source>
        <translation>يجري تنزيل النواه...</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="334"/>
        <source>Downloading Discord plugin</source>
        <translation>يجري تنزيل ملحق ديسكورد</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="202"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="112"/>
        <source>Downloading RetroArch</source>
        <translation>يجري تنزيل ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/updaters/self-updater.cpp" line="53"/>
        <source>Downloading installer...</source>
        <translation>يجري تنزيل المثبت...</translation>
    </message>
    <message>
        <location filename="../src/core/sdcard.mount.windows.cpp" line="423"/>
        <source>Downloading SD Card Manager Extension</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>EditableStarDisplayWidget</name>
    <message>
        <location filename="../src/rhdc/ui/star-display-widget.cpp" line="371"/>
        <source>Save slot is empty.</source>
        <translation>فتحة الحفظ فارغة.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/star-display-widget.cpp" line="375"/>
        <source>Create Save Slot</source>
        <translation>إنشاء فتحة حفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/star-display-widget.cpp" line="376"/>
        <source>Delete Save Slot</source>
        <translation>احذف  فتحة حفظ</translation>
    </message>
</context>
<context>
    <name>ErrorNotifier</name>
    <message>
        <location filename="../src/ui/error-notifier.cpp" line="5"/>
        <source>The application encountered an unexpected error:</source>
        <translation>واجه التطبيق خطأ غير متوقع:</translation>
    </message>
    <message>
        <location filename="../src/ui/error-notifier.cpp" line="12"/>
        <source>Unhandled Error</source>
        <translation>خطأ لا يمكن التعامل معه</translation>
    </message>
</context>
<context>
    <name>Game</name>
    <message>
        <location filename="../src/ui/play.cpp" line="45"/>
        <source>The emulator exited shortly after launching. If you are able to launch other ROMs without issues, then this ROM may contain invalid or unsafe RSP microcode that cannot be run on console accurate graphics plugins. Alternatively, if you have a very old onboard graphics card, it is possible that Vulkan is not supported on your system. In either case, using another graphics plugin might resolve the issue.</source>
        <translation>خرج المحاكي بعد وقت قصير من الإطلاق. إذا كنت قادرًا على تشغيل ROMs أخرى دون مشاكل ، فقد يحتوي هذا إلROM على رمز صغير RSP غير صالح أو غير آمن لا يمكن تشغيله على ملحقات رسومات دقيقة لوحدة ألعاب. بدلاً من ذلك ، إذا كان لديك بطاقة رسومات قديمة جدًا ، فمن المحتمل أن Vulkan غير مدعوم على نظامك. في كلتا الحالتين ، قد يؤدي استخدام مكون آخر للرسومات إلى حل المشكلة.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="51"/>
        <source>The emulator exited shortly after launching. If you are able to launch other ROMs without issues, then this ROM may contain invalid or unsafe RSP microcode that cannot be run on console accurate graphics plugins. If this is the case, try running the ROM with another graphics plugin instead.</source>
        <translation>خرج المحاكي بعد وقت قصير من الإطلاق. إذا كنت قادرًا على تشغيل ROMs أخرى دون مشاكل ، فقد يحتوي هذا إلROM على رمز صغير RSP غير صالح أو غير آمن لا يمكن تشغيله على ملحقات رسومات دقيقة لوحدة ألعاب. إذا كانت هذه هي الحالة ، فحاول تشغيل إلROM باستخدام مكون آخر للرسومات بدلاً من ذلك.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="56"/>
        <source>The emulator exited shortly after launching. If you are able to launch other ROMs without issues, then this ROM is most likely corrupt.</source>
        <translation>خرج المحاكي بعد وقت قصير من الإطلاق. إذا كنت قادرًا على تشغيل ROMs أخرى دون مشاكل ، فمن المرجح أن يكون هذا الROM تالفًا.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="271"/>
        <source>Emulator Missing</source>
        <translation>المحاكي مفقود</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="272"/>
        <source>Failed to launch emulator. It does not appear to be installed.</source>
        <translation>فشل تشغيل المحاكي. لا يبدو أنه مثبت.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="461"/>
        <source>Possible ROM Error</source>
        <translation>خطأ ROM محتمل</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="531"/>
        <source>Warning</source>
        <translation>تحذير</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="537"/>
        <source>Do not warn me again</source>
        <translation>لا تحذرني مرة أخرى</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="610"/>
        <source>This game uses an emulated Gamecube controller, but you have not bound any inputs to the X and Y buttons.</source>
        <translation>تستخدم هذه اللعبة وحدة تحكم Gamecube مقلدة ، لكنك لم تربط أي أدخال بأزرار X و Y.</translation>
    </message>
    <message>
        <location filename="../src/ui/play.cpp" line="620"/>
        <source>This game uses an emulated Gamecube controller, but you do not have an analog stick bound to the C buttons (C stick).</source>
        <translation>تستخدم هذه اللعبة وحدة تحكم Gamecube مقلدة ، ولكن ليس لديك عصا تناظرية مرتبطة بأزرار C (عصا C).</translation>
    </message>
</context>
<context>
    <name>GetControllerPortDialog</name>
    <message>
        <location filename="../src/ui/get-controller-port-dialog.cpp" line="8"/>
        <source>Choose Controller</source>
        <translation>إختر ذراع التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/get-controller-port-dialog.cpp" line="9"/>
        <source>Press any button on the controller to bind to this port.</source>
        <translation>اضغط على أي زر في ذراع التحكم للربط بهذا المنفذ.</translation>
    </message>
</context>
<context>
    <name>GroupName</name>
    <message>
        <location filename="../src/core/special-groups.cpp" line="14"/>
        <source>Favourites</source>
        <translation>المفضلأت</translation>
    </message>
    <message>
        <location filename="../src/core/special-groups.cpp" line="16"/>
        <source>Uncategorized</source>
        <translation>غير مصنف</translation>
    </message>
    <message>
        <location filename="../src/core/special-groups.cpp" line="18"/>
        <source>Want To Play</source>
        <translation>أريد أن ألعب</translation>
    </message>
    <message>
        <location filename="../src/core/special-groups.cpp" line="20"/>
        <source>In Progress</source>
        <translation>في إلتقدم</translation>
    </message>
    <message>
        <location filename="../src/core/special-groups.cpp" line="22"/>
        <source>Completed</source>
        <translation>منجز</translation>
    </message>
</context>
<context>
    <name>HotkeyEditWidget</name>
    <message>
        <location filename="../src/ui/hotkey-edit-widget.cpp" line="8"/>
        <source>None</source>
        <translation>منعدم</translation>
    </message>
</context>
<context>
    <name>ImportSaveDialog</name>
    <message>
        <location filename="../src/ui/import-save-dialog.cpp" line="10"/>
        <source>Select Save File</source>
        <translation>إختر ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/import-save-dialog.cpp" line="11"/>
        <source>Project64 Save Files</source>
        <translation>ملفات حفظ Project64</translation>
    </message>
    <message>
        <location filename="../src/ui/import-save-dialog.cpp" line="21"/>
        <source>Save data imported</source>
        <translation>تم إستيراد بيانات الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/import-save-dialog.cpp" line="24"/>
        <source>Failed to import save data</source>
        <translation>فشل إستيراد بيانات الحفظ</translation>
    </message>
</context>
<context>
    <name>ImportSdCardDialog</name>
    <message>
        <location filename="../src/ui/designer/import-sd-card-dialog.ui" line="17"/>
        <source>Import SD Card</source>
        <translatorcomment>there&apos;s no Arabic word for SD card</translatorcomment>
        <translation>استيراد بطاقة sd</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/import-sd-card-dialog.ui" line="31"/>
        <source>Name</source>
        <translation>اسم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/import-sd-card-dialog.ui" line="69"/>
        <source>Use this file directly</source>
        <translation>استخدم هذا الملف مباشرة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/import-sd-card-dialog.ui" line="79"/>
        <source>Make a copy of this file</source>
        <translation>قم بعمل نسخة من هذا الملف</translation>
    </message>
    <message>
        <location filename="../src/ui/import-sd-card-dialog.cpp" line="69"/>
        <source>Import</source>
        <translation>استيراد</translation>
    </message>
    <message>
        <location filename="../src/ui/import-sd-card-dialog.cpp" line="132"/>
        <source>This name is already in use</source>
        <translation>هذا الاسم قيد الاستخدام بالفعل</translation>
    </message>
    <message>
        <location filename="../src/ui/import-sd-card-dialog.cpp" line="149"/>
        <source>This name is reserved by Windows</source>
        <translatorcomment>Windows is generally left in English but can be </translatorcomment>
        <translation>هذا الاسم محجوز بواسطة ويندوز</translation>
    </message>
    <message>
        <location filename="../src/ui/import-sd-card-dialog.cpp" line="165"/>
        <source>Unexpected Error</source>
        <translation>خطأ غير متوقع</translation>
    </message>
    <message>
        <location filename="../src/ui/import-sd-card-dialog.cpp" line="165"/>
        <source>Failed to import SD card.</source>
        <translation>فشل استيراد بطاقة SD.</translation>
    </message>
</context>
<context>
    <name>InputModeSelect</name>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="514"/>
        <source>Normal</source>
        <translation>عادي</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="515"/>
        <source>Maps your gamepad inputs to a single N64 controller using your controller profile</source>
        <translation>تعين مدخلات ذراع التحكم الخاصة بك إلى وحدة تحكم N64 واحدة باستخدام ملف تعريف ذراع التحكم الخاص بك</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="538"/>
        <source>Dual Analog</source>
        <translation>مزدوج التناظرية</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="539"/>
        <source>Your gamepad inputs that normally bind to the C buttons instead bind to the analog stick on a second N64 controller</source>
        <translation>مدخلات ذراع التحكم الخاصة بك التي في العادي ترتبط بأزرار C سوف بدلاً من ذلك ترتبط بالعصا التناظرية على وحدة تحكم N64 ثانية</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="562"/>
        <source>GoldenEye</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="563"/>
        <source>Maps your gamepad inputs to two N64 controllers suitable for playing GoldenEye with the 2.4 Goodhead control style</source>
        <translation>تقوم بتعيين مدخلات ذراع الألعاب الخاصة بك إلى ذراعين تحكم N64 مناسبه للعب GoldenEye بنمط 2.4 Goodhead control</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="586"/>
        <source>Clone</source>
        <translation>استنسخ</translation>
    </message>
    <message>
        <location filename="../src/core/preset-controllers.cpp" line="587"/>
        <source>Your gamepad inputs are sent to two controller ports instead of just one</source>
        <translation>يتم إرسال مدخلات ذراع التحكم الخاصة بك إلى منفذين تحكم بدلاً من منفذ واحد فقط</translation>
    </message>
</context>
<context>
    <name>IsViewerWindow</name>
    <message>
        <location filename="../src/ui/is-viewer-window.cpp" line="58"/>
        <source>Error: IS Viewer was disconnected from the emulator.</source>
        <translatorcomment>What is IS viewer?</translatorcomment>
        <translation>خطأ: تم قطع اتصال IS Viewer بالمحاكي.</translation>
    </message>
    <message>
        <location filename="../src/ui/is-viewer-window.cpp" line="59"/>
        <source>Error: Failed to connect IS Viewer to the emulator.</source>
        <translation>خطأ: فشل في توصيل IS Viewer بالمحاكي.</translation>
    </message>
</context>
<context>
    <name>KeyboardConfigDialog</name>
    <message>
        <location filename="../src/ui/designer/keyboard-config-dialog.ui" line="14"/>
        <source>Configure Keyboard Controls</source>
        <translation>اضبط ضوابط لوحة المفاتيح</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/keyboard-config-dialog.ui" line="28"/>
        <source>Hotkeys</source>
        <translation>مفاتيح الاختصار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/keyboard-config-dialog.ui" line="71"/>
        <source>Keyboard Controls</source>
        <translation>ضوابط لوحة المفاتيح</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="18"/>
        <source>Analog Up</source>
        <translation>عصا التناظرية فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="19"/>
        <source>Analog Down</source>
        <translation>عصا التناظرية تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="20"/>
        <source>Analog Left</source>
        <translation>عصا التناظرية يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="21"/>
        <source>Analog Right</source>
        <translation>عصا التناظرية يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="22"/>
        <source>C Up</source>
        <translation>زر C فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="23"/>
        <source>C Down</source>
        <translation>زر C  تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="24"/>
        <source>C Left</source>
        <translation>زر C  يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="25"/>
        <source>C Right</source>
        <translation>زر C  يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="26"/>
        <source>D-Pad Up</source>
        <translation>زر الاتجاهات فوق</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="27"/>
        <source>D-Pad Down</source>
        <translation>زر الاتجاهات تحت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="28"/>
        <source>D-Pad Left</source>
        <translation>زر الاتجاهات يسار</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="29"/>
        <source>D-Pad Right</source>
        <translation>زر الاتجاهات يمين</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="30"/>
        <source>A Button</source>
        <translation>زر A</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="31"/>
        <source>B Button</source>
        <translation>زر B</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="32"/>
        <source>L Trigger</source>
        <translation>زناد L</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="33"/>
        <source>Z Trigger</source>
        <translation>زناد Z</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="34"/>
        <source>R Trigger</source>
        <translation>زناد R</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="35"/>
        <source>Start Button</source>
        <translation>زر البدء</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="36"/>
        <source>X Button</source>
        <translation>زر X</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="37"/>
        <source>Y Button</source>
        <translation>زر Y</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="39"/>
        <source>Save State</source>
        <translation>حفظ إلحالة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="40"/>
        <source>Load State</source>
        <translation>تحميل إلحالة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="41"/>
        <source>Previous State</source>
        <translation>الحالة السابقة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="42"/>
        <source>Next State</source>
        <translation>الحالة القادمة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="43"/>
        <source>Previous Cheat</source>
        <translation>الغشه السابقه</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="44"/>
        <source>Next Cheat</source>
        <translation>الغشه القادمة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="45"/>
        <source>Toggle Cheat</source>
        <translation>تبديل الغشه</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="46"/>
        <source>Toggle FPS Display</source>
        <translation>تبديل عرض الإطارات في الثانية</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="47"/>
        <source>Pause/Unpause</source>
        <translation>وقفة / إلغاء الإيقاف</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="48"/>
        <source>Frame Advance</source>
        <translation>تقدم الإطار</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="49"/>
        <source>Fast Forward (Hold)</source>
        <translation>تقديم سريع (ضغط مستمر)</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="50"/>
        <source>Fast Forward (Toggle)</source>
        <translation>تقديم سريع (تبديل)</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="51"/>
        <source>Slow Motion (Hold)</source>
        <translation>حركة بطيئة (ضغط مستمر)</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="52"/>
        <source>Slow Motion (Toggle)</source>
        <translation>حركة بطيئة (تبديل)</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="53"/>
        <source>Rewind*</source>
        <translation>ترجيع*</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="54"/>
        <source>Quit Emulator</source>
        <translation>قم بإنهاء المحاكي</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="55"/>
        <source>Hard Reset</source>
        <translation>إعادة التعيين الكلي</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="56"/>
        <source>Toggle Fullscreen</source>
        <translation>تبديل ملء الشاشة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="57"/>
        <source>RetroArch Menu</source>
        <translation>قائمة ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="58"/>
        <source>Record Video</source>
        <translation>سجل فيديو</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="59"/>
        <source>Record Inputs</source>
        <translation>سجل  إلمدخلات</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="60"/>
        <source>Take Screenshot</source>
        <translation>خد لقطة للشاشة</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="61"/>
        <source>Raise Volume</source>
        <translation>أرفع الصوت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="62"/>
        <source>Lower Volume</source>
        <translation>اخفض الصوت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="63"/>
        <source>Mute/Unmute</source>
        <translation>كتم الصوت / إلغاء كتم الصوت</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="64"/>
        <source>Grab Mouse</source>
        <translation>انتزع الماوس</translation>
    </message>
    <message>
        <location filename="../src/ui/keyboard-config-dialog.cpp" line="98"/>
        <source>You must enable rewind functionality in the RetroArch menu to use this hotkey.</source>
        <translation>يجب تمكين وظيفة الإرجاع في قائمة ريتروارش لاستخدام مفتاح التشغيل السريع هذا.</translation>
    </message>
</context>
<context>
    <name>LogViewerDialog</name>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="14"/>
        <source>Log Viewer</source>
        <translation>عارض سجل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="28"/>
        <source>Only show logs from this session</source>
        <translation>إعرض السجلات من هذه الجلسة فقط</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="48"/>
        <source>Show timestamps</source>
        <translation>اعرض الطوابع الزمنية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="60"/>
        <source>Visible Logs</source>
        <translation>سجلات مرئية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="66"/>
        <source>Debug</source>
        <translation>تنقيح</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="76"/>
        <source>Info</source>
        <translation>معلومات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="86"/>
        <source>Warn</source>
        <translation>تحذير</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="96"/>
        <source>Error</source>
        <translation>خطأ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/log-viewer-dialog.ui" line="106"/>
        <source>Fatal</source>
        <translation>فادح</translation>
    </message>
</context>
<context>
    <name>Main</name>
    <message>
        <location filename="../src/main.cpp" line="232"/>
        <source>Installing RetroArch</source>
        <translation>يجري تثبيت ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="233"/>
        <source>Parallel Launcher will now install RetroArch.</source>
        <translation>ستقوم Parallel Launcher الآن بتثبيت ريتروأرش.</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="242"/>
        <source>Fatal Error</source>
        <translation>خطأ فادح</translation>
    </message>
    <message>
        <location filename="../src/main.cpp" line="243"/>
        <source>Failed to install RetroArch.</source>
        <translation>فشل تثبيت ريتروارش.</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="64"/>
        <source>Refresh ROM List</source>
        <translation>انعش قائمة إلROMS</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="81"/>
        <source>Configure Controller</source>
        <translation>ضبط ذراع التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="98"/>
        <source>Options</source>
        <translation>خيارات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="128"/>
        <source>Searching for ROMs...</source>
        <translation>يجري البحث عن ROMs ...</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="185"/>
        <source>No ROMS have been found</source>
        <translation>لم يتم العثور على ROMS</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="192"/>
        <location filename="../src/ui/designer/main-window.ui" line="333"/>
        <source>Manage ROM Sources</source>
        <translation>إدير مصادر ROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="236"/>
        <source>Search...</source>
        <translation>ابحث...</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="283"/>
        <source>Download</source>
        <translation>تنزيل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="300"/>
        <source>Play Multiplayer</source>
        <translation>لعب متعددة اللاعبين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="311"/>
        <source>Play Singleplayer</source>
        <translation>لعب منفرد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="328"/>
        <source>Settings</source>
        <translation>الإعدادات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="338"/>
        <source>Configure Controllers</source>
        <translation>ضبط أذرعة التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="343"/>
        <source>Keyboard Controls and Hotkeys</source>
        <translation>ضوابط لوحة المفاتيح ومفاتيح الاختصار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="346"/>
        <source>Configure keyboard controls and hotkeys</source>
        <translation>اضبط ضوابط لوحة المفاتيح ومفاتيح الاختصار</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="351"/>
        <location filename="../src/ui/main-window.cpp" line="789"/>
        <source>Login to romhacking.com</source>
        <translation>تسجيل الدخول إلى romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="356"/>
        <source>Logout of romhacking.com</source>
        <translation>تسجيل الخروج من romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="361"/>
        <source>Disable romhacking.com integration</source>
        <translation>عطل تكامل romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="366"/>
        <source>Add Single ROM</source>
        <translation>ضيف ROM واحد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="375"/>
        <source>Quit Parallel Launcher</source>
        <translation>إنهاء  Parallel Launcher</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="380"/>
        <source>Open save file directory</source>
        <translation>افتح دليل حفظ الملف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="385"/>
        <source>Open savestate directory</source>
        <translatorcomment>There&apos;s no translation for savestate that I know of so I translated it literally </translatorcomment>
        <translation>افتح دليل حالة حفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="390"/>
        <source>Open SD card directory</source>
        <translation>افتح دليل بطاقة SD</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="395"/>
        <source>Open app data directory</source>
        <translation>افتح دليل بيانات التطبيق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="400"/>
        <source>Open app config directory</source>
        <translation>افتح دليل تكوين التطبيق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/main-window.ui" line="405"/>
        <source>Open app cache directory</source>
        <translation>افتح دليل ذاكرة التخزين المؤقت للتطبيق</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="144"/>
        <source>Data Directories</source>
        <translation>دلائل البيانات</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="631"/>
        <source>Select a ROM</source>
        <translation>إختر ROM</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="632"/>
        <source>ROM or Patch File</source>
        <translation>ملف ROM أو رقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="657"/>
        <location filename="../src/ui/main-window.cpp" line="660"/>
        <location filename="../src/ui/main-window.cpp" line="663"/>
        <location filename="../src/ui/main-window.cpp" line="666"/>
        <source>Patch Failed</source>
        <translation>فشل الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="657"/>
        <source>Failed to patch ROM. The .bps patch appears to be invalid.</source>
        <translation>فشل ترقيع إلROM. يبدو أن ترقيع إلbps غير صالح.</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="660"/>
        <source>Failed to patch ROM. The base ROM does not match what the patch expects.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي لا يتطابق مع ما يتوقعه الترقيع ..</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="663"/>
        <source>Failed to patch ROM. The base rom required to patch is not known to Parallel Launcher.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي المطلوب للترقيعه غير معروف لـ Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="666"/>
        <source>Failed to patch ROM. An error occurred while writing the patched ROM to disk.</source>
        <translation>فشل ترقيع إلROM. حدث خطأ أثناء كتابة إلROM المرقعه إلى  القرص.</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="789"/>
        <source>Enable romhacking.com integration</source>
        <translation>مكن تكامل romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="828"/>
        <source>Confirm Disable</source>
        <translation>أكد إلتعطيل</translation>
    </message>
    <message>
        <location filename="../src/ui/main-window.cpp" line="828"/>
        <source>Are you sure you want to disable romhacking.com integration?</source>
        <translation>هل أنت متأكد من أنك تريد تعطيل دمج romhacking.com؟</translation>
    </message>
</context>
<context>
    <name>ManageGroupsDialog</name>
    <message>
        <location filename="../src/ui/designer/manage-groups-dialog.ui" line="14"/>
        <source>Manage Groups</source>
        <translation>إدارة المجموعات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-groups-dialog.ui" line="32"/>
        <source>Delete</source>
        <translation>احذف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-groups-dialog.ui" line="43"/>
        <source>Rename</source>
        <translation>إعادة تسمية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-groups-dialog.ui" line="54"/>
        <source>New</source>
        <translation>جديد</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="55"/>
        <source>Confirm Delete</source>
        <translation>أكد الحذف</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="55"/>
        <source>Are you sure you want to delete this group?</source>
        <translation>هل أنت متأكد أنك تريد حذف هذه المجموعة؟</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="75"/>
        <source>Rename Group</source>
        <translation>غير تسمية المجموعة</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="75"/>
        <source>Enter a new name for your group</source>
        <translation>أدخل اسمًا جديدًا لمجموعتك</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="85"/>
        <source>Rename Failed</source>
        <translation>فشلت إعادة التسمية</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="85"/>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="104"/>
        <source>A group with this name already exists.</source>
        <translation>توجد بالفعل مجموعة بهذا الاسم.</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="99"/>
        <source>Create Group</source>
        <translation>إنشاء مجموعة</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="99"/>
        <source>Enter a name for your new group</source>
        <translation>أدخل اسمًا لمجموعتك الجديدة</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-groups-dialog.cpp" line="104"/>
        <source>Create Failed</source>
        <translation>فشل الإنشاء</translation>
    </message>
</context>
<context>
    <name>ManageSdCardsDialog</name>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="14"/>
        <source>Manage Virtual SD Cards</source>
        <translation>إدارة بطاقات SD الافتراضية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="42"/>
        <source>Create New</source>
        <translation>انشاء جديد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="53"/>
        <source>Import</source>
        <translation>استيراد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="111"/>
        <source>Size</source>
        <translation>سعة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="118"/>
        <source>Format</source>
        <translation>تهيئة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="147"/>
        <source>None</source>
        <translation>منعدم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="168"/>
        <source>Name</source>
        <translation>الاسم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="211"/>
        <source>Reformat</source>
        <translation>إعادة تهيئة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="222"/>
        <source>Delete</source>
        <translation>احذف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="251"/>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="423"/>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="445"/>
        <source>Clone</source>
        <translation>استنسخ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="262"/>
        <source>Browse Files</source>
        <translation>تصفح ملفات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="295"/>
        <source>Create</source>
        <translation>إنشاء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/manage-sd-cards-dialog.ui" line="306"/>
        <source>Cancel</source>
        <translation>إلغاء</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="125"/>
        <source>Confirm Reformat</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="126"/>
        <source>Are you sure you want to reformat this virtual SD card? All data currently on it will be lost.</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="221"/>
        <source>Confirm Deletion</source>
        <translation>أكد الحذف</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="222"/>
        <source>Are you sure you want to delete this virtual SD card? This action cannot be undone.</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="253"/>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="287"/>
        <source>Unexpected Error</source>
        <translation>خطأ غير متوقع</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="253"/>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="287"/>
        <source>Failed to create the virtual SD card because of an unknown error.</source>
        <translation>فشل إنشاء بطاقة SD الافتراضية بسبب خطأ غير معروف.</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="305"/>
        <source>Import SD Card</source>
        <translation>استيراد بطاقة sd</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="306"/>
        <source>Raw Disk Image</source>
        <translation>صورة القرص الخام</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="314"/>
        <source>Invalid disk image</source>
        <translation>صورة القرص غير صالحة</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="314"/>
        <source>The specified file is not a valid disk image.</source>
        <translation>الملف المحدد ليس صورة قرص صالحة.</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="317"/>
        <source>Disk image too large</source>
        <translation>صورة القرص كبيرة جدًا</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="317"/>
        <source>The disk image could not be imported because it is larger than 2 TiB.</source>
        <translation>لم يمكن استيراد صورة القرص لأنها أكبر من 2 تيرابايت.</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="404"/>
        <source>Operation Failed</source>
        <translation>فشلت العملية</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="405"/>
        <source>Failed to mount the SD card image. For more detailed error information, go back to the main window and press F7 to view error logs.</source>
        <translation>فشل تحميل صورة بطاقة SD. للحصول على معلومات أكثر تفصيلاً عن الأخطاء، ارجع إلى النافذة الرئيسية واضغط على F7 لعرض سجلات الأخطاء.</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="419"/>
        <source>Clone SD Card</source>
        <translation>استنساخ بطاقة SD</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="420"/>
        <source>Enter a name for the copied SD card:</source>
        <translation>أدخل اسمًا لبطاقة إلSD المنسوخة:</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="522"/>
        <source>Confirm Close</source>
        <translation>تأكيد الإغلاق</translation>
    </message>
    <message>
        <location filename="../src/ui/manage-sd-cards-dialog.cpp" line="523"/>
        <source>All SD cards you have opened for browsing will be closed. Continue?</source>
        <translation>سيتم إغلاق جميع بطاقات SD التي فتحتها للتصفح. إكمل؟</translation>
    </message>
</context>
<context>
    <name>MultiplayerControllerSelectDialog</name>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="14"/>
        <source>Select Controllers</source>
        <translation>إختر أذرعة التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="22"/>
        <source>Port 1</source>
        <translation>منفذ ١</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="33"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="59"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="85"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="111"/>
        <source>-- None --</source>
        <translation>-- منعدم --</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="41"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="67"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="93"/>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="119"/>
        <source>Detect Device</source>
        <translation>أكشف جهاز</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="48"/>
        <source>Port 2</source>
        <translation>منفذ ٢</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="74"/>
        <source>Port 3</source>
        <translation>منفذ ٣</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="100"/>
        <source>Port 4</source>
        <translation>منفذ ٤</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/multiplayer-controller-select-dialog.ui" line="128"/>
        <source>Allow port 1 to save and load states</source>
        <translation>اسمح للمنفذ 1 بحفظ وتحميل الحالات</translation>
    </message>
</context>
<context>
    <name>NeutralInputDialog</name>
    <message>
        <location filename="../src/ui/designer/neutral-input-dialog.ui" line="14"/>
        <source>Return to Neutral</source>
        <translation>العودة إلى الوضع المحايد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/neutral-input-dialog.ui" line="29"/>
        <source>Return all triggers and analog sticks to their neutral position, then press any button on the controller or click the OK button to continue.</source>
        <translation>أعد جميع الازندة  والعصي التناظرية إلى موضعها المحايد ، ثم اضغط على أي زر في ذراع التحكم أو انقر فوق الزر &quot;موافق&quot; للمتابعة.</translation>
    </message>
</context>
<context>
    <name>NowPlayingWidget</name>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="34"/>
        <source>Now Playing: </source>
        <translation>العب الان: </translation>
    </message>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="106"/>
        <source>Play Time</source>
        <translation>وقت اللعب</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="119"/>
        <source>Total</source>
        <translation>إجمالي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="150"/>
        <source>This Session</source>
        <translation>هذه إلجلسة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="221"/>
        <source>Hard Reset</source>
        <translation>إعادة التعيين الكلي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/now-playing-widget.ui" line="232"/>
        <source>Kill Emulator</source>
        <translation>اقتل المحاكي</translation>
    </message>
    <message>
        <location filename="../src/ui/now-playing-widget.cpp" line="18"/>
        <source>The hack author has not provided a star layout for this hack. This layout will show unattainable stars and may be missing some others.</source>
        <translation>لم يقدم مؤلف الاختراق تخطيط نجوم لهذا الاختراق. سيُظهر هذا التخطيط نجومًا لا يمكن الوصول إليها وقد يكون بعض النجوم الآخره مفقودة.</translation>
    </message>
    <message>
        <location filename="../src/ui/now-playing-window.cpp" line="177"/>
        <source>Parallel Launcher - Now Playing</source>
        <translation>العب الان - Parallel Launcher</translation>
    </message>
</context>
<context>
    <name>RetroArch</name>
    <message>
        <location filename="../src/core/retroarch.cpp" line="798"/>
        <source>[Parallel Launcher] Input driver encountered an unexpected error. Connected controllers may not be mapped correctly.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>RhdcDownloadDialog</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-download-dialog.ui" line="14"/>
        <source>Downloading Patch</source>
        <translation>يجري تنزيل الترقيعه</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-download-dialog.ui" line="25"/>
        <source>Fetching hack info</source>
        <translation>يجري جلب معلومات الاختراق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="158"/>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="252"/>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="412"/>
        <source>Failed to download patch from RHDC</source>
        <translation>فشل تنزيل الترقيعه من RHDC</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="314"/>
        <source>Downloading star layout</source>
        <translation>يجري تنزيل تخطيط النجوم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="343"/>
        <source>Failed to download star layout from RHDC</source>
        <translation>فشل تنزيل تخطيط النجوم من RHDC</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="363"/>
        <source>Downloading patch</source>
        <translation>يجري تنزيل الترقيعه</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="386"/>
        <source>Applying patch</source>
        <translation>يجري تطبيق الترقيعه</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="395"/>
        <source>The rom could not be installed because you do not have an unmodified copy of the US version of Super Mario 64 in your known roms list.</source>
        <translation>لم يمكن تثبيت إلrom لأن ليس لديك نسخة غير معدلة من إصدار الولايات المتحدة من Super Mario 64 في قائمة إلROMs المعروفة لديك.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="452"/>
        <source>The rom could not be installed because no bps patch was found.</source>
        <translation>لم يمكن تثبيت إلrom لأن لم يتم العثور على ترقيع bps.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="466"/>
        <source>Could not install &apos;%1&apos; because of an unexpected error while applying the patch</source>
        <translation>لم يمكن تثبيت %1 بسبب خطأ غير متوقع أثناء تطبيق الترقيع</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="476"/>
        <source>Computing checksum</source>
        <translation>يجري حساب المجموع الاختباري</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-download-dialog.cpp" line="498"/>
        <source>Romhack installed successfully</source>
        <translation>تم تثبيت إختراق إلROM  بنجاح</translation>
    </message>
</context>
<context>
    <name>RhdcHackView</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="30"/>
        <source>Hack List:</source>
        <translation>قائمة الاختراقت:</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="63"/>
        <source>Sort By:</source>
        <translation>صنف حسب:</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="80"/>
        <source>Name</source>
        <translation>الاسم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="85"/>
        <source>Popularity</source>
        <translation>إلشعبية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="90"/>
        <source>Rating</source>
        <translation>التقدير</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="95"/>
        <source>Difficulty</source>
        <translation>الصعوبة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="100"/>
        <source>Last Played</source>
        <translation>اخر مره لعبت</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="105"/>
        <source>Play Time</source>
        <translation>وقت اللعب</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="110"/>
        <source>Stars</source>
        <translation>النجوم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="115"/>
        <source>Fixed Shuffle</source>
        <extracomment>&quot;Fixed Shuffle&quot; means that the order of the roms is arbitrary, but will be consistent every time, as opposed to the &quot;Random Shuffle&quot; option which is randomized every time</extracomment>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="120"/>
        <source>Random Shuffle</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="141"/>
        <source>Search...</source>
        <translation>ابحث...</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack-view.ui" line="276"/>
        <source>Version</source>
        <translation>الإصدار</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="89"/>
        <source>All Hack Lists</source>
        <translation>قائمة جميع الإختراقات</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="140"/>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="146"/>
        <source>Authors:</source>
        <translation>المؤلفون:</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="141"/>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="152"/>
        <source>Unknown</source>
        <extracomment>Shown when no authors are listed for a hack</extracomment>
        <translation>مجهول</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="146"/>
        <source>Author:</source>
        <translation>إلمؤلف:</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="313"/>
        <source>Create List</source>
        <translation>إنشاء قائمة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="313"/>
        <source>Enter a name for your new hack list</source>
        <translation>أدخل اسمًا لقائمة الاختراقأت الجديدة الخاصة بك</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="318"/>
        <source>List Exists</source>
        <translation>القائمة موجودة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="318"/>
        <source>A hack list with this name already exists</source>
        <translation>قائمة اختراقأت بهذا الاسم موجودة بالفعل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="344"/>
        <source>Rate Hack</source>
        <translation>قيم الاختراق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="347"/>
        <source>Mark as Not Completed</source>
        <translation>ضع علامات &quot;لم ينجز&quot;</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="349"/>
        <source>Mark as Completed</source>
        <translation>ضع علامات &quot;منجز&quot;</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="359"/>
        <source>Move to Hack List...</source>
        <translation>أنقل إلى قائمة الاختراق ...</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="360"/>
        <source>Copy to Hack List...</source>
        <translation>أنسخ إلى قائمة الاختراق ...</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="367"/>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="368"/>
        <source>New List</source>
        <translation>قائمة جديدة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="374"/>
        <source>Remove from &apos;%1&apos;</source>
        <translation>ازيل من %1</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="381"/>
        <source>Delete Save File</source>
        <translation>احذف ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="384"/>
        <source>Edit Save File</source>
        <translation>حرر ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="385"/>
        <source>Import Project64 Save File</source>
        <translation>استيراد ملف حفظ Project64</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="386"/>
        <source>Show Save File</source>
        <translation>إظهار ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="390"/>
        <source>Unfollow Hack</source>
        <translation>إلغي متابعة الإختراق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="392"/>
        <source>Uninstall Hack Version</source>
        <translation>إلغاء تثبيت إصدار الإختراق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="487"/>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="523"/>
        <source>Confirm Delete</source>
        <translation>أكد الحذف</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="487"/>
        <source>Are you sure you want to delete your save file?</source>
        <translation>هل أنت متأكد أنك تريد حذف ملف الحفظ الخاص بك؟</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="514"/>
        <source>Confirm Unfollow</source>
        <translation>تأكيد إلغاء المتابعة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="514"/>
        <source>Are you sure you want to unfollow this hack? The ROM will not be deleted from your computer, but it will be removed from all of your hack lists on romhacking.com and your progress will no longer be synced with romhacking.com.</source>
        <translation>هل أنت متأكد أنك تريد إلغاء متابعة هذا الاختراق؟ لن يتم حذف إلROM من جهاز الكمبيوتر الخاص بك ، ولكن ستتم إزالته من جميع قوائم الاختراقأت الخاصة بك على romhacking.com ولن تتم مزامنة تقدمك مع romhacking.com.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-view.cpp" line="523"/>
        <source>Are you sure you want to delete this ROM file from your computer?</source>
        <translation>هل أنت متأكد أنك تريد حذف ملف إلROM هذا من جهاز الكمبيوتر الخاص بك؟</translation>
    </message>
</context>
<context>
    <name>RhdcHackWidget</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="302"/>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="341"/>
        <source>Popularity (Downloads)</source>
        <translation>الشعبية (التنزيلات)</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="385"/>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="424"/>
        <source>Rating</source>
        <translation>التقدير</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="465"/>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="504"/>
        <source>Difficulty</source>
        <translation>إلصعوبة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="561"/>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="600"/>
        <source>Last Played</source>
        <extracomment>Date and time that a hack was last played</extracomment>
        <translation>اخر مره لعبت</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="641"/>
        <location filename="../src/rhdc/ui/designer/rhdc-hack.ui" line="680"/>
        <source>Total Play Time</source>
        <translation>إجمالي وقت اللعب</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-widget.cpp" line="185"/>
        <source>Never</source>
        <extracomment>Text shown in place of a date when a hack hasn&apos;t been played yet</extracomment>
        <translation>أبداً</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-widget.cpp" line="198"/>
        <source>Completed</source>
        <translation>منجز</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-hack-widget.cpp" line="198"/>
        <source>Incomplete</source>
        <translation>غير منجز</translation>
    </message>
</context>
<context>
    <name>RhdcLoginDialog</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="14"/>
        <source>RHDC Login</source>
        <translation>تسجيل الدخول RHDC</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="66"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;To enable romhacking.com integration, you must have an unmodified copy of the US version of &lt;span style=&quot; font-style:italic;&quot;&gt;Super Mario 64&lt;/span&gt; in z64 format.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>لتمكين تكامل romhacking.com، يجب أن يكون لديك نسخة غير معدلة من إصدار الولايات المتحدة من Super Mario 64 بتنسيق .z64 </translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;‫لتمكين تكامل romhacking.com، يجب أن يكون لديك نسخة غير معدلة من إصدار الولايات المتحدة من &lt;span style=&quot; font-style:italic;&quot;&gt;Super Mario 64 &lt;/span&gt;بتنسيق إلz64. .&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="128"/>
        <source>Browse for SM64 ROM</source>
        <translation>تصفح من أجل SM64 ROM</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="180"/>
        <source>Sign in to romhacking.com</source>
        <translation>سجل الدخول إلى romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="194"/>
        <source>Username</source>
        <translation>اسم المستخدم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="204"/>
        <source>Password</source>
        <translation>كلمة المرور</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="256"/>
        <source>Sign In</source>
        <translation>سجل الدخول</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="359"/>
        <source>Multi-factor authentication is enabled on this account</source>
        <translatorcomment>Temporary translation until a better one is provided</translatorcomment>
        <translation>المصادقة الثنائية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="366"/>
        <source>Please enter the 6-digit authentication code from your authenticator app.</source>
        <translatorcomment>Temporary translation until a better one is provided</translatorcomment>
        <translation>إن حسابك محمي بواسطة المصادقة الثنائية. يُرجى إدخال الرمز المعروض في تطبيق المصادقة.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="437"/>
        <source>Verify</source>
        <translatorcomment>Temporary translation until a better one is provided</translatorcomment>
        <translation>تحقق من</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="501"/>
        <source>Incorrect authentication code</source>
        <translatorcomment>Temporary translation until a better one is provided</translatorcomment>
        <translation>تم إدخال الرمز بشكل غير صحيح</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="546"/>
        <source>Please confirm your romhacking.com integration settings. You can always change these later in the settings dialog.</source>
        <translation>يرجى تأكيد إعدادات تكامل romhacking.com. يمكنك دائمًا تغييرها لاحقًا في مربع حوار الإعدادات.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="574"/>
        <source>Download Directory</source>
        <translation>دليل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="596"/>
        <source>Browse</source>
        <translation>تصفح</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="620"/>
        <source>Sets the directory that hacks downloaded from romhacking.com will be saved to. If you change this setting later, your roms will automatically be moved to the new directory.</source>
        <translation>يضبط الدليل الذي سيتم حفظ الاختراق الذي تم تنزيله من romhacking.com فيه. إذا قمت بتغيير هذا الإعداد لاحقًا ، فسيتم تلقائيًا نقل إلROMs الخاص بك إلى الدليل الجديد.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="652"/>
        <source>Enable Star Display</source>
        <translation>تفعيل عرض النجوم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="673"/>
        <source>If checked, when a rom is running that has a supported layout file from RHDC, a layout of all the stars in the hack will be shown. This layout updates automatically after the game is saved.</source>
        <translation>إذا تم تحديده ، عند تشغيل rom به ملف تخطيط مدعوم من RHDC ، سيتم عرض تخطيط لجميع النجوم في الاختراق. يتم تحديث هذا التخطيط تلقائيًا بعد حفظ اللعبة.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="710"/>
        <source>Show star display for hacks without a star layout</source>
        <translation>إظهار عرض النجوم للاختراق بدون تخطيط نجوم</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="731"/>
        <source>If checked, Parallel Launcher will still show a star display with a default layout if the hack author did not provide one.</source>
        <translation>إذا تم تحديده ، فسيظل Parallel Launcher يعرض عرض النجوم بتخطيط افتراضي إذا لم يقدم مؤلف الاختراق واحدًا.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="768"/>
        <source>Check all save slots</source>
        <translation>تحقق من جميع فتحات الحفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="786"/>
        <source>If checked, Parallel Launcher will submit your highest star count across all save slots to romhacking.com; otherwise, it will only submit your star count in slot A.</source>
        <translation>إذا تم تحديده ، فسيقوم Parallel Launcher بإرسال أعلى عدد نجوم لك عبر جميع فتحات الحفظ إلى romhacking.com ؛ وإلا ، فسيتم إرسال عدد النجوم في الفتحة A  فقط.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="818"/>
        <source>Prefer HLE plugins</source>
        <translation>تفضل ملحقات HLE</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="836"/>
        <source>If checked, Parallel Launcher will use the GLideN64 plugin instead of the ParaLLEl plugin when it guesses that ParrLLEl is probably the best plugin to use. Only check this if your computer has an old GPU with poor Vulkan support. If you still experience lag even with GLideN64, you may need to disable &apos;Emulate Framebuffer&apos; and/or &apos;Emulate N64 Depth Compare&apos; in the GFX Plugins section of your Parallel Launcher settings.</source>
        <translation>إذا تم تحديده ، فسيستخدم Parallel Launcher الملحق GLideN64 بدلاً من الملحق ParaLLEl عندما يخمن أن ParrLLEl هو على الأرجح أفضل ملحق يمكن استخدامه. تحقق فقط من هذا إذا كان جهاز الكمبيوتر الخاص بك يحتوي على وحدة معالجة رسومات قديمة مع دعم Vulkan ضعيف. إذا كنت لا تزال تواجه تأخرًا حتى مع GLideN64 ، فقد تحتاج إلى تعطيل &quot;Emulate Framebuffer&quot; و / أو &quot;محاكاة مقارنة عمق N64&quot; في قسم ملحقات GFX في إعدادات Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="868"/>
        <source>Ignore widescreen hint</source>
        <translation>تجاهل تلميح الشاشة العريضة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-login-dialog.ui" line="886"/>
        <source>If checked, Parallel Launcher will always default to using 4:3 resolution, even if the recommended settings indicate that widescreen is supported</source>
        <translation>إذا تم تحديده ، فسيظل Parallel Launcher دائمًا لاستخدام دقة 4: 3 بشكل إفتراضي ، حتى إذا كانت الإعدادات الموصى بها تشير إلى أن الشاشة العريضة مدعومة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="79"/>
        <source>Select SM64 ROM</source>
        <translation>إختر SM64 ROM</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="85"/>
        <source>ROM Does not Match</source>
        <translation>إلROM لا يتطابق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="85"/>
        <source>The provided rom does not match the expected checksum.</source>
        <translation>لا يتطابق rom المقدم مع المجموع الاختباري المتوقع.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="127"/>
        <source>Select a new download directory</source>
        <translation>إختر دليل تنزيل جديد</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="150"/>
        <source>Enter your username</source>
        <translation>أدخل اسم المستخدم الخاص بك</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="155"/>
        <source>Enter your password</source>
        <translation>ادخل كلمة المرور الخاصة بك</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="206"/>
        <source>Username or password is incorrect</source>
        <translation>اسم المستخدم أو كلمة المرور غير صحيحة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="209"/>
        <source>The romhacking.com servers appear to be down.</source>
        <translation>يبدو أن خوادم romhacking.com معطلة.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-login-dialog.cpp" line="212"/>
        <source>An unknown error occurred.</source>
        <translation>حدث خطأ غير معروف.</translation>
    </message>
</context>
<context>
    <name>RhdcRatingDialog</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-rating-dialog.ui" line="14"/>
        <source>Rate this Hack</source>
        <translation>قيم هذا الإختراق</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-rating-dialog.ui" line="25"/>
        <source>How would you rate this hack?</source>
        <translation>كيف تقيم هذا الاختراق؟</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-rating-dialog.ui" line="51"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Quality. &lt;/span&gt;How enjoyable, impressive, and/or polished was this hack?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>إلجودة: إلى أي مدى كان هذا الاختراق ممتعًا ومثيرًا للإعجاب و / أو مصقولًا؟ </translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;إلجودة. &lt;/span&gt;إلى أي مدى كان هذا الاختراق ممتعًا ومثيرًا للإعجاب و / أو مصقولًا؟&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-rating-dialog.ui" line="174"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Difficulty. &lt;/span&gt;In general, how would you rate this hack&apos;s difficulty level?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>الصعوبة: بشكل عام ، كيف تقيم مستوى صعوبة هذا الاختراق؟</translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt; الصعوبة. &lt;/span&gt;بشكل عام ، كيف تقيم مستوى صعوبة هذا الاختراق؟&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="10"/>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="24"/>
        <source>I can&apos;t decide or have no opinion</source>
        <extracomment>0</extracomment>
        <translation>لا أستطيع أن أقرر أو ليس لدي رأي</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="11"/>
        <source>No Challenge</source>
        <extracomment>1</extracomment>
        <translation>عديم التحدي</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="12"/>
        <source>Very Easy</source>
        <extracomment>2</extracomment>
        <translation>سهل جدا</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="13"/>
        <source>Casual</source>
        <extracomment>3</extracomment>
        <translation>عفوية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="14"/>
        <source>Classic SM64</source>
        <extracomment>4</extracomment>
        <translation>كلاسيكية SM64</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="15"/>
        <source>Moderate</source>
        <extracomment>5</extracomment>
        <translation>معتدل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="16"/>
        <source>Challenging</source>
        <extracomment>6</extracomment>
        <translation>متحدية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="17"/>
        <source>Difficult</source>
        <extracomment>7</extracomment>
        <translation>صعبة</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="18"/>
        <source>Very Difficult</source>
        <extracomment>8</extracomment>
        <translation>صعبة جدا</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="19"/>
        <source>Extremely Difficult</source>
        <extracomment>9</extracomment>
        <translation>صعب للغاية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="20"/>
        <source>Borderline Kaizo</source>
        <extracomment>10</extracomment>
        <translatorcomment>There&apos;s no translation for Kaizo, but it is transliterated on Arabic gaming sites</translatorcomment>
        <translation>يقترب من إلكايزو</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="25"/>
        <source>Beginner/Introductory Kaizo</source>
        <extracomment>1</extracomment>
        <translation>كايزو مبتدئ / تمهيدي</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="26"/>
        <source>Easy Kaizo</source>
        <extracomment>2</extracomment>
        <translation>كايزو سهل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="27"/>
        <source>Standard Kaizo</source>
        <extracomment>3</extracomment>
        <translation>كايزو تقليدي</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="28"/>
        <source>Moderate Kaizo</source>
        <extracomment>4</extracomment>
        <translation>كايزو معتدل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="29"/>
        <source>Challenging Kaizo</source>
        <extracomment>5</extracomment>
        <translation>كايزو متحدي</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="30"/>
        <source>Difficult Kaizo</source>
        <extracomment>6</extracomment>
        <translation>كايزو صعب</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="31"/>
        <source>Very Difficult Kaizo</source>
        <extracomment>7</extracomment>
        <translation>كايزو صعب جدا</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="32"/>
        <source>Extremely Difficult Kaizo</source>
        <extracomment>8</extracomment>
        <translation>كايزو صعب للغاية</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="33"/>
        <source>Hardest humanly possible Kaizo</source>
        <extracomment>9</extracomment>
        <translation>اصعب كايزو يقدر عليه انسان</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="34"/>
        <source>TAS/Unbeatable</source>
        <extracomment>10</extracomment>
        <translation>غير قابل للهزم / TAS</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="45"/>
        <source>Submit</source>
        <translation>إرسل</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/rhdc-rating-dialog.cpp" line="76"/>
        <source>Not Rated</source>
        <translation>لم تقيم</translation>
    </message>
</context>
<context>
    <name>RhdcSaveEditor</name>
    <message>
        <location filename="../src/rhdc/ui/rhdc-save-editor.cpp" line="92"/>
        <source>Save Editor</source>
        <translation>محرر الحفظ</translation>
    </message>
</context>
<context>
    <name>RhdcSaveSyncDialog</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-save-sync-dialog.ui" line="14"/>
        <source>Synchronize Save Files</source>
        <translation>مزامنة ملفات الحفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-save-sync-dialog.ui" line="32"/>
        <source>Save file synchronization is enabled for this hack; however, your save file progress is not currently in sync. Please select which save file you would like to use for this hack.</source>
        <translation>مزامنة ملف الحفظ لهذا الاختراق مفعلة؛ ومع ذلك ، فإن تقدم ملف الحفظ الخاص بك ليس متزامنًا حاليًا. يرجى تحديد ملف الحفظ الذي ترغب في استخدامه لهذا الاختراق.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-save-sync-dialog.ui" line="67"/>
        <source>Version</source>
        <extracomment>Which version of the hack the save file is for</extracomment>
        <translation>الإصدار</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-save-sync-dialog.ui" line="72"/>
        <source>Last Saved</source>
        <extracomment>When the save file was last modified</extracomment>
        <translation>آخر حفظ</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-save-sync-dialog.ui" line="77"/>
        <source>Stars</source>
        <extracomment>Number of stars collected on the save file</extracomment>
        <translation>نجوم</translation>
    </message>
</context>
<context>
    <name>RhdcSync</name>
    <message>
        <location filename="../src/rhdc/core/sync.cpp" line="235"/>
        <source>Failed to connect to romhacking.com</source>
        <translation>فشل الاتصال بـ romhacking.com</translation>
    </message>
</context>
<context>
    <name>RhdcViewBubble</name>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-view-bubble.ui" line="171"/>
        <source>Click here to go to the romhacking.com view.</source>
        <translation>انقر هنا للذهاب إلى عرض romhacking.com.</translation>
    </message>
    <message>
        <location filename="../src/rhdc/ui/designer/rhdc-view-bubble.ui" line="214"/>
        <source>Okay</source>
        <translation>حسنا</translation>
    </message>
</context>
<context>
    <name>RomListModel</name>
    <message>
        <location filename="../src/ui/rom-list-model.cpp" line="243"/>
        <source>Name</source>
        <translation>اسم</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-model.cpp" line="244"/>
        <source>Internal Name</source>
        <translation>إلاسم الداخلي</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-model.cpp" line="245"/>
        <source>File Path</source>
        <translation>مسار الملف</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-model.cpp" line="246"/>
        <source>Last Played</source>
        <translation>اخر مره لعبت</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-model.cpp" line="247"/>
        <source>Play Time</source>
        <translation>وقت اللعب</translation>
    </message>
</context>
<context>
    <name>RomListView</name>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="236"/>
        <source>Delete Save File</source>
        <translation>احذف ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="237"/>
        <source>[SM64] Edit Save File</source>
        <translation>[SM64] حرر ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="240"/>
        <source>Import Project64 Save File</source>
        <translation>استيراد ملف حفظ Project64</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="241"/>
        <source>Show Save File</source>
        <translation>إظهار ملف الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="246"/>
        <source>Download</source>
        <translation>تنزيل</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="250"/>
        <source>Add to...</source>
        <translation>أضف إلى...</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="257"/>
        <source>New Group</source>
        <translation>مجموعة جديدة</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="261"/>
        <source>Remove from %1</source>
        <translation>ازيل من %1</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="267"/>
        <source>Open Containing Folder</source>
        <translation>افتح المجلد الحاوي</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="268"/>
        <source>Delete ROM</source>
        <translation>احذف إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="270"/>
        <location filename="../src/ui/rom-list-view.cpp" line="373"/>
        <source>Rename</source>
        <translation>إعادة تسمية</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="272"/>
        <source>Revert ROM Name</source>
        <translation>استرجع اسم إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="277"/>
        <source>Rate Hack</source>
        <translation>قيم الاختراق</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="280"/>
        <source>Mark as Not Completed</source>
        <translation>ضع علامات &quot;لم ينجز&quot;</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="282"/>
        <source>Mark as Completed</source>
        <translation>ضع علامات &quot;منجز&quot;</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="305"/>
        <location filename="../src/ui/rom-list-view.cpp" line="364"/>
        <source>Confirm Delete</source>
        <translation>أكد الحذف</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="305"/>
        <source>Are you sure you want to delete your save file?</source>
        <translation>هل أنت متأكد أنك تريد حذف ملف الحفظ الخاص بك؟</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="325"/>
        <source>Create Group</source>
        <translation>إنشاء مجموعة</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="325"/>
        <source>Enter a name for your new group</source>
        <translation>أدخل اسمًا لمجموعتك الجديدة</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="331"/>
        <source>Group Exists</source>
        <translation>المجموعة موجودة</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="331"/>
        <source>A group with this name already exists</source>
        <translation>توجد بالفعل مجموعة بهذا الاسم</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="364"/>
        <source>This will completely remove the ROM file from your computer. Are you sure you want to delete this ROM?</source>
        <translation>سيؤدي هذا إلى إزالة ملف إلROM تمامًا من جهاز الكمبيوتر الخاص بك. هل أنت متأكد أنك تريد حذف هذا إلROM؟</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-list-view.cpp" line="373"/>
        <source>Enter a new name for your rom</source>
        <translation>أدخل اسمًا جديدً للROM الخاصة بك</translation>
    </message>
</context>
<context>
    <name>RomSettingsWidget</name>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="20"/>
        <source>Sync save files between hack versions</source>
        <translation>مزامنة ملفات الحفظ بين إصدارات الاختراق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="29"/>
        <source>Input Mode</source>
        <translation>وضع الإدخال</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="76"/>
        <source>SD Card</source>
        <translation>كارت SD</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="156"/>
        <source>Unlocks the CPU to run at full speed, removing CPU-based lag you would encounter on console. This is almost always safe, but can rarely cause issues on certain ROMs.</source>
        <translation>يسمح لوحدة المعالجة المركزية التشغيل بأقصى سرعة ، ويزيل التأخر بسبب وحدة المعالجة المركزية الذي قد تواجهه على وحدة إلعب الأصليه. هذا آمن دائمًا تقريبًا ، ولكن نادرًا ما يتسبب في حدوث مشاكل في بعض إلROMs.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="159"/>
        <location filename="../src/ui/rom-settings-widget.cpp" line="171"/>
        <source>Overclock CPU (Recommended)</source>
        <translation>ارفع تردد التشغيل لوحدة المعالجة المركزية (موصى به)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="175"/>
        <source>Alters the vertical interrupt timing. In most cases this either gives a slight performance boost or has no effect, but in some cases it can cause the game to run at the wrong framerate.</source>
        <translation>يغير توقيت المقاطعة العمودية. في معظم الحالات ، يؤدي هذا إما إلى زيادة طفيفة في الأداء أو ليس له أي تأثير ، ولكن في بعض الحالات يمكن أن يتسبب في تشغيل اللعبة بمعدل إطار خاطئ.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="178"/>
        <source>Overclock VI</source>
        <translation>ارفع تردد تشغيل VI</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="191"/>
        <source>Overrides save type autodetection, and forces the use of 16kB EEPROM for save data. This breaks roms that use 4kB EEPROM, so only enable this for romhacks that require it.</source>
        <translation>يتجاوز حفظ نوع الاكتشاف التلقائي ، ويفرض استخدام 16kB EEPROM لبيانات الحفظ. هذا يكسر إلROMs التي تستخدم 4kB EEPROM ، لذلك قم فقط بتمكين هذا من أجل إختراقات إلROMs التي تتطلب ذلك.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="194"/>
        <source>Force 16kB EEPROM</source>
        <translation>فرض 16kB EEPROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="207"/>
        <source>Use the interpreter core instead of the dynamic recompiler. Slower, but can be useful for debugging.</source>
        <translation>استخدم نواة المفسر بدلاً من برنامج إعادة الترجمه الديناميكي.هذا أبطأ ، ولكن يمكن أن يكون مفيدًا في التصحيح.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="210"/>
        <source>Use Interpreter Core</source>
        <translation>استخدم نواة المفسر</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="223"/>
        <source>Enable widescreen mode by stretching the video. Some ROMs have a widescreen mode that will allow the video to no longer be stretched when it is enabled.</source>
        <translation>قم بتمكين وضع الشاشة العريضة عن طريق تمديد الفيديو. تحتوي بعض إلROMs على وضع الشاشة العريضة الذي سيسمح للفيديو بعدم التمدد عند تمكينه.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="226"/>
        <source>Widescreen (Stretched)</source>
        <translation>شاشة عريضة (ممتدة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="239"/>
        <source>Enable widescreen mode by forcing the game to use a widescreen viewport. You will experience visual artifacts on the sides of the screen unless you are playing a romhack specifically designed for this widescreen mode.</source>
        <translation>قم بتمكين وضع الشاشة العريضة من خلال إجبار اللعبة على استخدام Viewport بشاشة عريضة. ستختبر القطع الأثرية المرئية على جوانب الشاشة إلا إذا كنت تلعب إختراق ROM مصمم خصيصًا لوضع الشاشة العريضة هذا.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="242"/>
        <source>Widescreen (Viewport Hack)</source>
        <translation>شاشة عريضة (إختراق Viewport)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="255"/>
        <source>Upscale textures drawn using the TEX_RECT command. This can cause visual artifacts in some games.</source>
        <translation>قم بإلترقية قوام الرسومات باستخدام امر TEX_RECT. يمكن أن يتسبب هذا في حدوث عيوب بصرية في بعض الألعاب.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="258"/>
        <source>Upscale TEXRECTs</source>
        <translation>قم بترقية TEXRECTs</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="271"/>
        <source>Removes the black borders on the left and right sides of the video. Since these pixels are never rendered on real hardware, results will vary depending on the game.</source>
        <translation>يزيل الحدود السوداء على الجانبين الأيمن والأيسر من الفيديو. نظرًا لأن وحدات البكسل هذه لا تُعرض أبدًا على إلأجهزة إلحقيقية ، فستختلف النتائج اعتمادًا على اللعبة.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="274"/>
        <source>Remove Black Borders</source>
        <translation>إزالة الحدود السوداء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="287"/>
        <source>Emulate the native framebuffer. This is required for some visual effects to work, but may cause lag on lower end GPUs</source>
        <translation>محاكاة مخزن الإطار المؤقت الأصلي. هذا مطلوب حتى تعمل بعض التأثيرات المرئية ، ولكنه قد يتسبب في تأخير في وحدات معالجة الرسومات ذات قدرة منخفضة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="290"/>
        <source>Emulate Framebuffer</source>
        <translation>محاكاة مخزن الإطار المؤقت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="306"/>
        <source>Greatly increases accuracy by rendering decals correctly, but may cause a loss in performance</source>
        <translation>يزيد الدقة بشكل كبير من خلال عرض الشارات بشكل صحيح ، ولكنه قد يتسبب في فقدان الأداء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="309"/>
        <source>Emulate N64 Depth Compare</source>
        <translation>محاكاة مقارنة عمق N64</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="325"/>
        <source>Allows roms with custom RSP microcode to be played, but causes visible seams in models due to plugin inaccuracies.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="348"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Warning: &lt;/span&gt;Not enabling CPU overclocking is likely to result in a laggy experience. This option should only be used for speedruns or testing where you want to approximate console CPU lag.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>تحذير: من المحتمل أن يؤدي عدم تمكين رفع تردد التشغيل لوحدة المعالجة المركزية إلى تجربة فيها تأخر . ينصح استخدام هذا الخيار فقط لمسابقات السرعة أو للاختبار حيث تريد تقدير تأخر وحدة المعالجة المركزية بوحدة الألعاب</translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;‫تحذير: &lt;/span&gt;من المحتمل أن يؤدي عدم تمكين رفع تردد التشغيل لوحدة المعالجة المركزية إلى تجربة فيها تأخر . ينصح استخدام هذا الخيار فقط لمسابقات السرعة أو للاختبار حيث تريد تقدير تأخر وحدة المعالجة المركزية بوحدة الألعاب .&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="373"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Warning:&lt;/span&gt; The &lt;span style=&quot; font-style:italic;&quot;&gt;Emulate N64 Depth Compare&lt;/span&gt; option does not work on MacOS. Some things may not render correctly.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>تحذير: اختيار محاكاة مقارنة عمق N64 لا يعمل على نظام MacOS. قد لا يتم عرض بعض الأشياء بشكل صحيح</translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;‫تحذير:&lt;/span&gt; اختيار   &lt;span style=&quot; font-style:italic;&quot;&gt;محاكاة مقارنة عمق N64&lt;/span&gt; لا يعمل على نظام MacOS. قد لا يتم عرض بعض الأشياء بشكل صحيح&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="398"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Warning:&lt;/span&gt; This ROM supports mouse input, but you do not have a hotkey assigned to &lt;span style=&quot; font-style:italic;&quot;&gt;Grab Mouse&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>تحذير: يدعم هذا إلROM إدخال الماوس ، ولكن ليس لديك مفتاح اختصار معين إلى انتزع الماوس</translatorcomment>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;‫تحذير:&lt;/span&gt;يدعم هذا إلROM إدخال الماوس ، ولكن ليس لديك مفتاح اختصار معين إلى &lt;span style=&quot; font-style:italic;&quot;&gt;انتزع الماوس&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="475"/>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="592"/>
        <source>Graphics Plugin</source>
        <translation>ملحق الرسومات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="484"/>
        <location filename="../src/ui/rom-settings-widget.cpp" line="539"/>
        <source>ParaLLEl (Recommended - very accurate to console)</source>
        <translation>ملحق ParaLLEl (موصى به - دقيقة جدًا لوحدة ألعاب)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="497"/>
        <location filename="../src/ui/rom-settings-widget.cpp" line="540"/>
        <source>GLideN64 (Recommended for lower end computers)</source>
        <translation>ملحق GLideN64 (موصى به لأجهزة الكمبيوتر ذات قدرة منخفضة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="507"/>
        <location filename="../src/ui/rom-settings-widget.cpp" line="541"/>
        <source>OGRE (Needed by some older romhacks)</source>
        <translation>ملحق OGRE (مطلوب من قبل بعض إختراقات إلROM الأقدم)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="557"/>
        <source>gln64 (Obsolete)</source>
        <translation>ملحق gln64 (مهمل)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-settings-widget.ui" line="601"/>
        <source>ParaLLEl (Recommended)</source>
        <translation>ملحق ParaLLEl (موصى به)</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="171"/>
        <source>Overclock CPU</source>
        <translation>ارفع تردد التشغيل لوحدة المعالجة المركزية</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="331"/>
        <source>Show Fewer Plugins</source>
        <translation>إعرض عدد أقل من الملحقات</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="331"/>
        <source>Show More Plugins</source>
        <translation>عرض المزيد من الملحقات</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="342"/>
        <source>This ROM supports mouse input. After launching the emulator, your mouse will be captured by the emulator. Press %1 if you need to free the mouse cursor.</source>
        <translation>يدعم هذا إلROM إدخال الماوس. بعد تشغيل المحاكي، سيتم التقاط الماوس الخاص بك بواسطة المحاكي. اضغط %1 إذا كنت تريد تحرير مؤشر الماوس.</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="521"/>
        <source>(recommended by hack author)</source>
        <translation>(موصى به بواسطة مؤلف الاختراق)</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-settings-widget.cpp" line="613"/>
        <source>None</source>
        <translation>منعدم</translation>
    </message>
</context>
<context>
    <name>RomSourceDialog</name>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="14"/>
        <source>Manage ROM Sources</source>
        <translation>إدير مصادر ROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="31"/>
        <source>ROM Search Folders</source>
        <translation>مجلدات بحث الROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="68"/>
        <source>Delete</source>
        <translation>احذف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="82"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="77"/>
        <source>New Source</source>
        <translation>مصدر جديد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="103"/>
        <source>ROM Source</source>
        <translation>مصدر ROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="114"/>
        <source>Browse for a folder</source>
        <translation>تصفح عن مجلد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="121"/>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="431"/>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="502"/>
        <source>Browse</source>
        <translation>تصفح</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="134"/>
        <source>Also scan all subfolders</source>
        <translation>افحص أيضًا كافة المجلدات الفرعية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="137"/>
        <source>Recursive</source>
        <translation>تكراري</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="168"/>
        <source>Ignore directories that begin with a period character</source>
        <translation>تجاهل الدلائل التي تبدأ بالنقطة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="171"/>
        <source>Ignore hidden directories</source>
        <translation>تجاهل الدلائل المخفية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="183"/>
        <source>Max Depth</source>
        <translation>أقصى عمق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="236"/>
        <source>Follow symbolic links to folders and ROMs</source>
        <translation>اتبع الروابط الرمزية للمجلدات و إلROMs</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="239"/>
        <source>Follow Symlinks</source>
        <translation>اتبع  الروابط الرمزية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="256"/>
        <source>Automatically add to groups</source>
        <translation>ضيف إلى المجموعات تلقائيًا</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="289"/>
        <source>Manage Groups</source>
        <translation>إدارة المجموعات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="317"/>
        <source>Individual ROMs</source>
        <translation>إلROMs إلفردية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="348"/>
        <source>Forget Selected ROM</source>
        <translation>إنسى إلROM المختاره</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="359"/>
        <source>Add New ROM(s)</source>
        <translation>ضيف ROM واحد أو أكثر جديد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="388"/>
        <source>BPS Patches</source>
        <translation>رقأعإت BPS</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="417"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="298"/>
        <source>Patch File</source>
        <translation>ملف الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="444"/>
        <source>Base ROM</source>
        <translation>إلROM الأساسي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="450"/>
        <source>Automatic (Search Existing ROMs)</source>
        <translation>تلقائي (البحث في ROM موجودة بالفعل)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="463"/>
        <source>Provided ROM:</source>
        <translation>إلROM المقدمة:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/rom-source-dialog.ui" line="532"/>
        <source>Apply Patch</source>
        <translation>طبق الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="275"/>
        <source>Select one or more ROMs</source>
        <translation>إختر ROM واحد أو أكثر</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="297"/>
        <source>Select a .bps patch file</source>
        <translation>إختر ملف رقعه .bps</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="308"/>
        <source>Select a ROM</source>
        <translation>إختر ROM</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="337"/>
        <source>Patch Applied</source>
        <translation>تم تطبيق الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="337"/>
        <source>Saved patched rom to %1</source>
        <translation>تم حفظ إلrom المرقعة إلى ـ%1</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="352"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="355"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="359"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="361"/>
        <location filename="../src/ui/rom-source-dialog.cpp" line="365"/>
        <source>Patch Failed</source>
        <translation>فشل الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="352"/>
        <source>Failed to patch ROM. The .bps patch appears to be invalid.</source>
        <translation>فشل ترقيع إلROM. يبدو أن ترقيع إلbps غير صالح.</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="355"/>
        <source>Failed to patch ROM. The base ROM does not match what the patch expects.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي لا يتطابق مع ما يتوقعه الترقيع ..</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="359"/>
        <source>Failed to patch ROM. The base rom required to patch is not known to Parallel Launcher.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي المطلوب للترقيعه غير معروف لـ Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="361"/>
        <source>Failed to patch ROM. The base ROM is missing or invalid.</source>
        <translation>فشل ترقيع إلROM. إلROM الأساسي مفقود أو غير صالح.</translation>
    </message>
    <message>
        <location filename="../src/ui/rom-source-dialog.cpp" line="365"/>
        <source>Failed to patch ROM. An error occurred while writing the patched ROM to disk.</source>
        <translation>فشل ترقيع إلROM. حدث خطأ أثناء كتابة إلROM المرقعه إلى  القرص.</translation>
    </message>
</context>
<context>
    <name>SaveFileEditorDialog</name>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="14"/>
        <source>Edit SM64 Save File</source>
        <translation>حرر ملف حفظ SM64</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="29"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;Warning: &lt;/span&gt;This is a save file editor for Super Mario 64 and SM64 romhacks. Attempting to use this save file editor with other ROMs will corrupt your save file.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translatorcomment>تحذير: هذا محرر ملف لملفات حفظ Super Mario 64 وإختراقات SM64 . ستؤدي محاولة استخدام محرر حفظ الملف هذا مع ROMs أخرى إلى إتلاف ملف الحفظ.</translatorcomment>
        <translation>&lt;html&gt; &lt;head /&gt; &lt;body&gt; &lt;p&gt; &lt;span style = &quot;font-weight: 600؛&quot;&gt;‫ تحذير: &lt;/ span&gt; هذا محرر ملف لملفات حفظ Super Mario 64 وإختراقات  SM64 . ستؤدي محاولة استخدام محرر حفظ الملف هذا مع ROMs  أخرى إلى إتلاف ملف الحفظ. &lt;/ p&gt; &lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="41"/>
        <source>Save Slot:</source>
        <translation>فتحة حفظ:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="55"/>
        <source>Slot A (Empty)</source>
        <translation>الفتحة A (فارغة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="60"/>
        <source>Slot B (Empty)</source>
        <translation>الفتحة B (فارغة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="65"/>
        <source>Slot C (Empty)</source>
        <translation>الفتحة C (فارغة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="70"/>
        <source>Slot D (Empty)</source>
        <translation>الفتحة D (فارغة)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="95"/>
        <source>Delete Save Slot</source>
        <translation>احذف  فتحة حفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-file-editor-dialog.ui" line="106"/>
        <location filename="../src/ui/save-file-editor-dialog.cpp" line="62"/>
        <source>Edit Save Slot</source>
        <translation>حرر فتحة حفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/save-file-editor-dialog.cpp" line="15"/>
        <source>Slot %1 (Empty)</source>
        <translation>الفتحة %1 (فارغة)</translation>
    </message>
    <message>
        <location filename="../src/ui/save-file-editor-dialog.cpp" line="18"/>
        <source>Slot %1 (%2 Stars)</source>
        <translation>الفتحة%1 (%2 نجوم)</translation>
    </message>
    <message>
        <location filename="../src/ui/save-file-editor-dialog.cpp" line="66"/>
        <source>Create Save Slot</source>
        <translation>إنشاء فتحة حفظ</translation>
    </message>
</context>
<context>
    <name>SaveSlotEditorDialog</name>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="14"/>
        <source>Edit Save Slot (SM64)</source>
        <translation>حرر فتحة حفظ [SM64]</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="20"/>
        <source>Show flags and stars unused in the vanilla game</source>
        <translation>أظهر الأعلام والنجوم إلغير مستخدمة في اللعبة الفانيليا</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="32"/>
        <source>Flags</source>
        <translation>الأعلام</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="74"/>
        <source>Caps Unlocked</source>
        <translation>قبعات غير مقفلة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="80"/>
        <source>Wing Cap</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="93"/>
        <source>Metal Cap</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="106"/>
        <source>Vanish Cap</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="122"/>
        <source>Keys Collected</source>
        <translation>مفاتيح إلمجمعة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="128"/>
        <source>Basement Key</source>
        <translation>مفتاح البدروم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="141"/>
        <source>Upstairs Key</source>
        <translation>مفتاح الطابق العلوي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="157"/>
        <source>Unlocked Doors</source>
        <translation>أبواب غير مقفلة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="163"/>
        <source>Basement Key Door</source>
        <translation>باب مفتاح البدروم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="176"/>
        <source>Upstairs Key Door</source>
        <translation>باب مفتاح الطابق العلوي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="189"/>
        <source>Peach&apos;s Secret Slide Door</source>
        <translatorcomment>You can leave &quot;Peach&apos;s Secret Slide&quot; untranslated</translatorcomment>
        <translation>باب Peach&apos;s Secret Slide</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="202"/>
        <source>Whomp&apos;s Fortress Door</source>
        <translatorcomment>You can leave &quot;Whomp&apos;s Fortress&quot; untranslated</translatorcomment>
        <translation>باب Whomp&apos;s Fortress</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="215"/>
        <source>Cool Cool Mountain Door</source>
        <translatorcomment>You can leave &quot;Cool Cool Mountain&quot; untranslated</translatorcomment>
        <translation>باب Cool Cool Mountain</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="228"/>
        <source>Jolly Rodger Bay Door</source>
        <translatorcomment>You can leave &quot;Jolly Rodger Bay&quot; untranslated</translatorcomment>
        <translation>باب Jolly Rodger Bay</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="241"/>
        <source>Dark World Door</source>
        <translatorcomment>You can leave &quot;Dark World&quot; untranslated</translatorcomment>
        <translation>باب Dark World</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="254"/>
        <source>Fire Sea Door</source>
        <translatorcomment>You can leave &quot;Fire Sea&quot; untranslated</translatorcomment>
        <translation>باب Fire Sea</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="267"/>
        <source>50 Star Door</source>
        <translation>باب ٥٠ نجمة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="283"/>
        <source>Miscellaneous</source>
        <translation>متفرقات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="289"/>
        <source>Moat Drained</source>
        <translation>تجفف الخندق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="302"/>
        <source>Bowser&apos;s Sub Gone</source>
        <translation>ذهبت غواصة Bowser</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="318"/>
        <source>Unused</source>
        <translation>غير مستعمل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="366"/>
        <source>Lost/Stolen Cap</source>
        <translation>قبعه مفقوده / مسروقه</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="374"/>
        <source>Level</source>
        <translation>المرحلة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="395"/>
        <source>Area</source>
        <translation>منطقة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="427"/>
        <source>Cap Stolen By:</source>
        <translation>القبعه مسروقه بواسطة:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="434"/>
        <source>The flag indicates that Mario&apos;s cap is on the ground, however, when loading a save file, a cap that is on the ground is either given back to Mario or moved to the appropriate NPC for the area, so it will never actually be on the ground.</source>
        <translation>يشير العلم إلى أن قبعة ماريو موجوده على الأرض ، ومع ذلك ، عند تحميل ملف حفظ ، يتم إعادة القبعة الموجوده على الأرض إلى ماريو أو يتم نقله إلى الشخصية المناسب للمنطقة ، لذلك لن تكون القبعة في الواقع على أرض.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="437"/>
        <source>Automatic*</source>
        <translation>تلقائي *</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="450"/>
        <source>Klepto (Bird)</source>
        <translatorcomment>Yeah I can&apos;t translate this name. </translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="463"/>
        <source>Ukiki (Monkey)</source>
        <translatorcomment>Yeah I can&apos;t translate this name. </translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="476"/>
        <source>Mr. Blizzard (Snowman)</source>
        <translatorcomment>Yeah I can&apos;t translate this name. </translatorcomment>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/save-slot-editor-dialog.ui" line="499"/>
        <source>Stars and Coins</source>
        <translation>نجوم وعملات</translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="44"/>
        <source>Big Boo&apos;s Haunt</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="45"/>
        <source>Cool Cool Mountain</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="46"/>
        <source>Castle Interior</source>
        <translation>القلعة الداخلية</translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="47"/>
        <source>Hazy Maze Cave</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="48"/>
        <source>Shifting Sand Land</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="49"/>
        <source>Bob-Omb Battlefield</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="50"/>
        <source>Snowman&apos;s Land</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="51"/>
        <source>Wet Dry World</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="52"/>
        <source>Jolly Rodger Bay</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="53"/>
        <source>Tiny-Huge Island</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="54"/>
        <source>Tick Tock Clock</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="55"/>
        <source>Rainbow Ride</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="56"/>
        <source>Castle Grounds</source>
        <translation>أراضي القلعة</translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="57"/>
        <source>Bowser in the Dark World</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="58"/>
        <source>Vanish Cap Under the Moat</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="59"/>
        <source>Bowser in the Fire Sea</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="60"/>
        <source>Secret Aquarium</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="61"/>
        <source>Bowser in the Sky</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="62"/>
        <source>Lethal Lava Land</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="63"/>
        <source>Dire Dire Docks</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="64"/>
        <source>Whomp&apos;s Fortress</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="65"/>
        <source>End Screen</source>
        <translation>شاشة النهاية</translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="66"/>
        <source>Castle Courtyard</source>
        <translation>فناء القلعة</translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="67"/>
        <source>Peach&apos;s Secret Slide</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="68"/>
        <source>Cavern of the Metal Cap</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="69"/>
        <source>Tower of the Wing Cap</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="70"/>
        <source>Bowser 1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="71"/>
        <source>Winged Mario over the Rainbow</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="73"/>
        <source>Bowser 2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="74"/>
        <source>Bowser 3</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/core/sm64.cpp" line="76"/>
        <source>Tall, Tall Mountain</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="11"/>
        <source>8 Red Coins</source>
        <translation>٨ عملات حمراء</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="12"/>
        <source>100 Coins</source>
        <translation>١٠٠ عملة</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="17"/>
        <source>Toad 1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="18"/>
        <source>Toad 2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="19"/>
        <source>Toad 3</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="20"/>
        <source>MIPS 1</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="21"/>
        <source>MIPS 2</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="27"/>
        <source>Big Bob-Omb on the Summit</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="28"/>
        <source>Footface with Koopa the Quick</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="29"/>
        <source>Shoot to the Island in the Sky</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="30"/>
        <source>Find the 8 Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="31"/>
        <source>Mario Wings to the Sky</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="32"/>
        <source>Behind Chain Chomp&apos;s Gate</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="37"/>
        <source>Chip Off Whomp&apos;s Block</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="38"/>
        <source>To the Top of the Fortress</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="39"/>
        <source>Shoot into the Wild Blue</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="40"/>
        <source>Red Coins on the Floating Isle</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="41"/>
        <source>Fall onto the Caged Island</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="42"/>
        <source>Blast Away the Wall</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="47"/>
        <source>Plunder in the Sunken Ship</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="48"/>
        <source>Can the Eel Come Out to Play?</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="49"/>
        <source>Treasure of the Ocean Cave</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="50"/>
        <source>Red Coins on the Ship Afloat</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="51"/>
        <source>Blast to the Stone Pillar</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="52"/>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="110"/>
        <source>Through the Jet Stream</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="57"/>
        <source>Slip Slidin&apos; Away</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="58"/>
        <source>Li&apos;l Penguin Lost</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="59"/>
        <source>Big Penguin Race</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="60"/>
        <source>Frosty Slide for 8 Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="61"/>
        <source>Snowman&apos;s Lost his Head</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="62"/>
        <source>Wall Kicks Will Work</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="67"/>
        <source>Go on a Ghost Hunt</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="68"/>
        <source>Ride Big Boo&apos;s Merry-Go-Round</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="69"/>
        <source>Secret of the Haunted Books</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="70"/>
        <source>Seek the 8 Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="71"/>
        <source>Big Boo&apos;s Balcony</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="72"/>
        <source>Eye to Eye in the Secret Room</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="77"/>
        <source>Swimming Beast in the Cavern</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="78"/>
        <source>Elevate for 8 Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="79"/>
        <source>Metal-Head Mario Can Move!</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="80"/>
        <source>Navigating the Toxic Maze</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="81"/>
        <source>A-maze-ing Emergency Exit</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="82"/>
        <source>Watch for Rolling Rocks</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="87"/>
        <source>Boil the Big Bully</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="88"/>
        <source>Bully the Bullies</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="89"/>
        <source>8-Coin Puzzle with 15 Pieces</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="90"/>
        <source>Red-Hot Log Rolling</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="91"/>
        <source>Hot-Foot-it into the Volcano</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="92"/>
        <source>Elevator Tour in the Volcano</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="97"/>
        <source>In the Talons of the Big Bird</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="98"/>
        <source>Shining Atop the Pyramid</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="99"/>
        <source>Inside the Ancient Pyramid</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="100"/>
        <source>Stand Tall on the Four Pillars</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="101"/>
        <source>Free Flying for 8 Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="102"/>
        <source>Pyramid Puzzle</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="107"/>
        <source>Board Bowser&apos;s Sub</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="108"/>
        <source>Chests in the Current</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="109"/>
        <source>Pole-Jumping for Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="111"/>
        <source>The Manta Ray&apos;s Reward</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="112"/>
        <source>Collect the Caps...</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="117"/>
        <source>Snowman&apos;s Big Head</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="118"/>
        <source>Chill with the Bully</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="119"/>
        <source>In the Deep Freeze</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="120"/>
        <source>Whirl from the Freezing Pond</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="121"/>
        <source>Shell Shreddin&apos; for Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="122"/>
        <source>Into the Igloo</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="127"/>
        <source>Shocking Arrow Lifts!</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="128"/>
        <source>Top o&apos; the Town</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="129"/>
        <source>Secrets in the Shallows &amp; Sky</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="130"/>
        <source>Express Evelator--Hurry Up!</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="131"/>
        <source>Go to Town for Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="132"/>
        <source>Quick Race through Downtown!</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="137"/>
        <source>Scale the Mountain</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="138"/>
        <source>Mystery of the Monkey&apos;s Cage</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="139"/>
        <source>Scary &apos;Shrooms, Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="140"/>
        <source>Mysterious Mountainside</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="141"/>
        <source>Breathtaking View from Bridge</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="142"/>
        <source>Blast to the Lonely Mushroom</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="147"/>
        <source>Pluck the Piranha Flower</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="148"/>
        <source>The Tip Top of the Huge Island</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="149"/>
        <source>Rematch with Koopa the Quick</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="150"/>
        <source>Five Itty Bitty Secrets</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="151"/>
        <source>Wiggler&apos;s Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="152"/>
        <source>Make Wiggler Squirm</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="157"/>
        <source>Roll into the Cage</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="158"/>
        <source>The Pit and the Pendulums</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="159"/>
        <source>Get a Hand</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="160"/>
        <source>Stomp on the Thwomp</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="161"/>
        <source>Timed Jumps on Moving Bars</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="162"/>
        <source>Stop Time for Red Coins</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="167"/>
        <source>Cruiser Crossing the Rainbow</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="168"/>
        <source>The Big House in the Sky</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="169"/>
        <source>Coins Amassed in a Maze</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="170"/>
        <source>Swingin&apos; in the Breeze</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="171"/>
        <source>Tricky Triangles!</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="172"/>
        <source>Somewhere Over the Rainbow</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="227"/>
        <source>Act 1 Star</source>
        <translation>نجمة الفصل ١</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="228"/>
        <source>Act 2 Star</source>
        <translation>نجمة الفصل ٢</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="229"/>
        <source>Act 3 Star</source>
        <translation>نجمة الفصل ٣</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="230"/>
        <source>Act 4 Star</source>
        <translation>نجمة الفصل ٤</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="231"/>
        <source>Act 5 Star</source>
        <translation>نجمة الفصل ٥</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="232"/>
        <source>Act 6 Star</source>
        <translation>نجمة الفصل ٦</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="233"/>
        <source>100 Coin Star</source>
        <translation>نجمة ١٠٠ عملة</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="234"/>
        <source>Cannon Status</source>
        <translation>حالة المدفع</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="235"/>
        <source>Coin High Score</source>
        <translation>أعلى نقاط العملات</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="264"/>
        <source>Peach&apos;s Castle</source>
        <translation>قلعة Peach</translation>
    </message>
    <message>
        <location filename="../src/ui/save-slot-editor-dialog.cpp" line="317"/>
        <source>-- None --</source>
        <translation>-- منعدم --</translation>
    </message>
</context>
<context>
    <name>SettingsDialog</name>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="14"/>
        <source>Settings</source>
        <translation>الإعدادات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="44"/>
        <source>User Interface</source>
        <translation>واجهة المستخدم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="49"/>
        <source>Emulation</source>
        <translation>محاكاة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="54"/>
        <source>BPS Patches</source>
        <translation>رقأعإت BPS</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="59"/>
        <source>GFX Plugins</source>
        <translation>ملحقات GFX</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="64"/>
        <source>Updaters</source>
        <translation>المحدثين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="69"/>
        <source>Romhacking.com</source>
        <translatorcomment>as is, urls are not translated even in arabic.</translatorcomment>
        <translation>Romhacking.com</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="118"/>
        <source>Theme (Requires Restart)</source>
        <translation>السمة (يتطلب إعادة التشغيل)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="134"/>
        <source>Dark Mode (Requires Restart)</source>
        <translation>إلنمط إلداكن (يتطلب إعادة التشغيل)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="143"/>
        <source>Language (Requires Restart)</source>
        <translation>اللغة (يتطلب إعادة التشغيل)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="158"/>
        <source>Hide launcher while playing ROM</source>
        <translation>إخفي المشغل أثناء تشغيل إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="165"/>
        <source>Check for updates on startup</source>
        <translation>تحقق من وجود تحديثات عند بدء التشغيل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="175"/>
        <source>Discord &quot;Now Playing&quot; Integration</source>
        <translation>تكامل &quot;العب الان&quot; لديسكورد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="182"/>
        <source>Show additional rom settings meant for advanced users</source>
        <translation>عرض إعدادات إلROM الإضافية المخصصة للمستخدمين المتقدمين</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="185"/>
        <source>Show advanced ROM options</source>
        <translation>إعرض خيارات إلROM المتقدمة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="192"/>
        <source>Visible Columns</source>
        <translation>أعمدة مرئية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="198"/>
        <source>The full path to the file</source>
        <translation>المسار الكامل للملف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="201"/>
        <source>File Path</source>
        <translation>مسار الملف</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="211"/>
        <source>The filename without the extension</source>
        <translation>اسم الملف بدون الامتداد</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="214"/>
        <source>Name</source>
        <translation>الاسم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="227"/>
        <source>The name stored in the ROM itself</source>
        <translation>الاسم المخزن في إلROM نفسه</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="230"/>
        <source>Internal Name</source>
        <translation>إلاسم الداخلي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="237"/>
        <source>The date you last played the ROM</source>
        <translation>تاريخ آخر مرة قمت فيها بلعب إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="240"/>
        <source>Last Played</source>
        <translation>اخر مره لعبت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="250"/>
        <source>The total time spent playing the ROM</source>
        <translation>إجمالي الوقت المستغرق في لعب هذأ إلROM</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="253"/>
        <source>Total Play Time</source>
        <translation>إجمالي وقت اللعب</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="438"/>
        <source>Window Scale</source>
        <translation>مقياس النافذة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="409"/>
        <source>Default ParallelN64 Plugin</source>
        <translation>ملحق ParallelN64 الافتراضي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="448"/>
        <source>Default Mupen Plugin</source>
        <translation>ملحق Mupen الافتراضي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="385"/>
        <source>Default Emulator Core</source>
        <translation>نواة المحاكي الافتراضي</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="74"/>
        <source>Developer</source>
        <translatorcomment>If you want to go with just developer, then it&apos;s مطور  however, if you want to go with &quot;hack developer&quot; then it&apos;s مطور الاختراق or &quot;software developer&quot; would be مطور برامج</translatorcomment>
        <translation>مطور الاختراق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="79"/>
        <source>System Clock</source>
        <translation>ساعة النظام</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="458"/>
        <source>Audio Driver</source>
        <translation>مشغّل الصّوت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="486"/>
        <source>Fullscreen</source>
        <translation>ملء الشاشة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="493"/>
        <source>Vsync</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="500"/>
        <source>Pause when emulator loses focus</source>
        <translation>وقف المحاكاه عندما يفقد المحاكي التركيز</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="507"/>
        <source>Do not show RetroArch notifications such as &quot;Saved state to slot #0&quot; or &quot;Device disconnected from port #1&quot;</source>
        <translation>لا تعرض اخطارات ريتروارش مثل &quot;الحالة المحفوظة في الفتحة رقم 0&quot; أو &quot;تم فصل الجهاز عن المنفذ رقم 1&quot;</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="510"/>
        <source>Hide notifications</source>
        <translation>إخفي الإخطارات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="527"/>
        <source>Emulate SummerCart64 SD card interface</source>
        <translation>محاكاة واجهة بطاقة SD الخاصة بـ SummerCart64</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="542"/>
        <source>Reset RetroArch Config</source>
        <translation>إعادة تعيين تكوين ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="594"/>
        <source>When loading a BPS patch, save the patched ROM...</source>
        <translation>عند تحميل ترقيع BPS ، احفظ إلROM المرقعه ...</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="601"/>
        <source>To the same folder as the patch</source>
        <translation>إلى نفس المجلد مثل الرقعه</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="614"/>
        <source>To this folder:</source>
        <translation>إلى هذا المجلد:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="656"/>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1355"/>
        <source>Browse</source>
        <translation>تصفح</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="691"/>
        <source>Global Settings</source>
        <translation>الاعدادات العامة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="715"/>
        <source>Upscaling</source>
        <translation>الارتقاء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="731"/>
        <source>Native (x1 - 320x240)</source>
        <translation>أصلي (x1 - 320x240)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="769"/>
        <location filename="../src/ui/designer/settings-dialog.ui" line="898"/>
        <source>Anti-Aliasing</source>
        <translation>إزالة التسنن</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="804"/>
        <source>Filtering</source>
        <translation>تصفية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="815"/>
        <source>None</source>
        <translation>منعدم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="820"/>
        <source>Anti-Alias</source>
        <translation>إزالة التسنن</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="825"/>
        <source>Anti-Alias + Dedither</source>
        <translation>إزالة التسنن + Dedither</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="830"/>
        <source>Anti-Alias + Blur</source>
        <translation>إزالة التسنن + طمس</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="835"/>
        <source>Filtered</source>
        <translation>تمت تصفيته</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="908"/>
        <source>Use N64 3-Point Filtering</source>
        <translation>استخدم تصفية N64 ثلاثية النقاط</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="932"/>
        <source>Default ROM Settings</source>
        <translation>إعدادات إلROM الافتراضية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="956"/>
        <source>Upscale textures drawn using the TEX_RECT command. This can cause visual artifacts in some games.</source>
        <translation>قم بإلترقية قوام الرسومات باستخدام امر TEX_RECT. يمكن أن يتسبب هذا في حدوث عيوب بصرية في بعض الألعاب.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="959"/>
        <source>Upscale TEXRECTs</source>
        <translation>قم بترقية TEXRECTs</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="979"/>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1101"/>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1145"/>
        <source>Apply to all existing ROMs</source>
        <translation>طبق على جميع إلROMs الموجودة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="997"/>
        <source>Removes the black borders on the left and right sides of the video. Since these pixels are never rendered on real hardware, results will vary depending on the game.</source>
        <translation>يزيل الحدود السوداء على الجانبين الأيمن والأيسر من الفيديو. نظرًا لأن وحدات البكسل هذه لا تُعرض أبدًا على إلأجهزة إلحقيقية ، فستختلف النتائج اعتمادًا على اللعبة.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1000"/>
        <source>Remove Black Borders</source>
        <translation>إزالة الحدود السوداء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1075"/>
        <source>Emulate the native framebuffer. This is required for some visual effects to work, but may cause lag on lower end GPUs</source>
        <translation>محاكاة مخزن الإطار المؤقت الأصلي. هذا مطلوب حتى تعمل بعض التأثيرات المرئية ، ولكنه قد يتسبب في تأخير في وحدات معالجة الرسومات ذات قدرة منخفضة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1078"/>
        <source>Emulate Framebuffer</source>
        <translation>محاكاة مخزن الإطار المؤقت</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1119"/>
        <source>Greatly increases accuracy by rendering decals correctly, but may cause a loss in performance</source>
        <translation>يزيد الدقة بشكل كبير من خلال عرض الشارات بشكل صحيح ، ولكنه قد يتسبب في فقدان الأداء</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1122"/>
        <source>Emulate N64 Depth Compare</source>
        <translation>محاكاة مقارنة عمق N64</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1182"/>
        <source>Enable RetroArch Automatic Updates</source>
        <translation>قم بتمكين التحديثات التلقائية لـريتروارش</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1192"/>
        <source>Enable Mupen64Plus-Next Automatic Updates</source>
        <translation>قم بتمكين التحديثات التلقائية لـMupen64Plus-Next</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1220"/>
        <source>Use development branch</source>
        <translation>استخدام فرع التنمية</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1234"/>
        <source>Update Interval</source>
        <translation>الفاصل الزمني للتحديث</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1254"/>
        <source>Every Launch</source>
        <translation>كل إطلاق</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1259"/>
        <source>Daily</source>
        <translation>يوميا</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1264"/>
        <source>Weekly</source>
        <translation>أسبوعيا</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1269"/>
        <source>Monthly</source>
        <translation>شهريا</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1294"/>
        <source>Check for Updates</source>
        <translation>تحقق من وجود تحديثات</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1333"/>
        <source>Download Directory</source>
        <translation>دليل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1345"/>
        <source>Sets the directory that hacks downloaded from romhacking.com will be saved to. If you change this setting later, your roms will automatically be moved to the new directory.</source>
        <translation>يضبط الدليل الذي سيتم حفظ الاختراق الذي تم تنزيله من romhacking.com فيه. إذا قمت بتغيير هذا الإعداد لاحقًا ، فسيتم تلقائيًا نقل إلROMs الخاص بك إلى الدليل الجديد.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1384"/>
        <source>Enable Star Display</source>
        <translation>تفعيل عرض النجوم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1413"/>
        <source>If checked, Parallel Launcher will still show a star display with a default layout if the hack author did not provide one.</source>
        <translation>إذا تم تحديده ، فسيظل Parallel Launcher يعرض عرض النجوم بتخطيط افتراضي إذا لم يقدم مؤلف الاختراق واحدًا.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1416"/>
        <source>Show star display for hacks without a
star layout</source>
        <translation>إظهار عرض النجوم للاختراقأت بدون ملف
تخطيط نجوم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1430"/>
        <source>If checked, Parallel Launcher will submit your highest star count across all save slots to romhacking.com; otherwise, it will only submit your star count in slot A.</source>
        <translation>إذا تم تحديده ، فسيقوم Parallel Launcher بإرسال أعلى عدد نجوم لك عبر جميع فتحات الحفظ إلى romhacking.com ؛ وإلا ، فسيتم إرسال عدد النجوم في الفتحة A  فقط.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1433"/>
        <source>Check all save slots</source>
        <translation>تحقق من جميع فتحات الحفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1440"/>
        <source>If checked, Parallel Launcher will use the GLideN64 plugin instead of the ParaLLEl plugin when it guesses that ParrLLEl is probably the best plugin to use. Only check this if your computer has an old GPU with poor Vulkan support. If you still experience lag even with GlideN64, you may need to disable &apos;Emulate Framebuffer&apos; and/or &apos;Emulate N64 Depth Compare&apos; in the GFX Plugins section of your Parallel Launcher settings.</source>
        <translation>إذا تم تحديده ، فسيستخدم Parallel Launcher الملحق GLideN64 بدلاً من الملحق ParaLLEl عندما يخمن أن ParrLLEl هو على الأرجح أفضل ملحق يمكن استخدامه. تحقق فقط من هذا إذا كان جهاز الكمبيوتر الخاص بك يحتوي على وحدة معالجة رسومات قديمة مع دعم Vulkan ضعيف. إذا كنت لا تزال تواجه تأخرًا حتى مع GLideN64 ، فقد تحتاج إلى تعطيل &quot;Emulate Framebuffer&quot; و / أو &quot;محاكاة مقارنة عمق N64&quot; في قسم ملحقات GFX في إعدادات Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1443"/>
        <source>Prefer HLE plugins</source>
        <translation>تفضل ملحقات HLE</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1450"/>
        <source>If checked, Parallel Launcher will always default to using 4:3 resolution, even if the recommended settings indicate that widescreen is supported</source>
        <translation>إذا تم تحديده ، فسيظل Parallel Launcher دائمًا لاستخدام دقة 4: 3 بشكل إفتراضي ، حتى إذا كانت الإعدادات الموصى بها تشير إلى أن الشاشة العريضة مدعومة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1453"/>
        <source>Ignore widescreen hint</source>
        <translation>تجاهل تلميح الشاشة العريضة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1477"/>
        <source>Emulate IS Viewer hardware and display any messages sent from the ROM in a separate window</source>
        <translation>محاكاة جهاز IS Viewer وعرض أي رسائل مرسلة من ذاكرة القراءة فقط (ROM) في نافذة منفصلة</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1480"/>
        <source>Enable IS Viewer (ParallelN64 only)</source>
        <translation>تمكين IS Viewer (Parallel N64 فقط)</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1508"/>
        <source>History Size</source>
        <translation>حجم التاريخ</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1577"/>
        <source>The following options only apply to the ParallelN64 emulator core. The mupen64plus core will always sync to the system clock.</source>
        <translation>تنطبق الخيارات التالية فقط على نواة محاكي ParallelN64. سيتزامن نواة mupen64plus دائمًا مع ساعة النظام.</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1593"/>
        <source>N64 Real Time Clock</source>
        <translation>ساعة الوقت الحقيقي N64</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1600"/>
        <source>Sync to system clock</source>
        <translation>مزامنة مع ساعة النظام</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1613"/>
        <source>Use this time:</source>
        <translation>استخدم هذا الوقت:</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/settings-dialog.ui" line="1653"/>
        <source>Rollback clock when loading a savestate</source>
        <translation>تراجع الساعة عند تحميل حالة حفظ</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="82"/>
        <source>Automatic (%1)</source>
        <translation>تلقائي (%1)</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="330"/>
        <source>Install Discord Plugin?</source>
        <translation>تثبيت ملحق ديسكورد؟</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="330"/>
        <source>You have enabled Discord integration, but the Discord plugin is not currently installed. Would you like to install it now?</source>
        <translation>لقد قمت بتمكين تكامل ديسكورد، لكن ملحق ديسكورد غير مثبت حاليًا. هل ترغب في تثبيته الان؟</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="340"/>
        <source>Download Failed</source>
        <translation>فشل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="340"/>
        <source>Failed to download Discord plugin</source>
        <translation>فشل تنزيل ملحق ديسكورد</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="374"/>
        <source>Auto</source>
        <translation>تلقائي</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="378"/>
        <location filename="../src/ui/settings-dialog.cpp" line="385"/>
        <source>Select a Folder</source>
        <translation>إختر مجلدا</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="394"/>
        <source>Confirm Reset</source>
        <translation>أكد إعادة التعيين</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="395"/>
        <source>This will reset your RetroArch config file, undoing any changes you have made within RetroArch. Your Parallel Launcher settings will not be affected. Do you want to continue?</source>
        <translation>سيؤدي هذا إلى إعادة تعيين ملف تكوين ريتروارش الخاص بك، والتراجع عن أي تغييرات قمت بإجرائها داخل ريتروارش. لن تتأثر إعدادات Parallel Launcher. هل تريد الاستمرار؟</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="402"/>
        <source>Config Reset</source>
        <translation>إعادة تعيين التكوين</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="402"/>
        <source>Your RetroArch config has been reset.</source>
        <translation>تمت إعادة تعيين تكوين ريتروارش الخاص بك.</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="404"/>
        <source>Oops</source>
        <translation>عفوا</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="404"/>
        <source>An unknown error occurred. Your RetroArch config has not been reset.</source>
        <translation>حدث خطأ غير معروف. لم تتم إعادة تعيين تكوين ريتروارش الخاص بك.</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="423"/>
        <location filename="../src/ui/settings-dialog.cpp" line="429"/>
        <location filename="../src/ui/settings-dialog.cpp" line="435"/>
        <location filename="../src/ui/settings-dialog.cpp" line="441"/>
        <source>Confirm Apply All</source>
        <translation>أكد تطبيق الكل</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="423"/>
        <location filename="../src/ui/settings-dialog.cpp" line="429"/>
        <location filename="../src/ui/settings-dialog.cpp" line="435"/>
        <location filename="../src/ui/settings-dialog.cpp" line="441"/>
        <source>Apply this setting to all current roms?</source>
        <translation>طبق هذا الإعداد على جميع إلROMs الحالية؟</translation>
    </message>
    <message>
        <location filename="../src/ui/settings-dialog.cpp" line="453"/>
        <source>Unlimited</source>
        <translation>غير محدود</translation>
    </message>
</context>
<context>
    <name>SingleplayerControllerSelectDialog</name>
    <message>
        <location filename="../src/ui/designer/singleplayer-controller-select-dialog.ui" line="20"/>
        <source>Controller Select</source>
        <translation>إختيار ذراع التحكم</translation>
    </message>
    <message>
        <location filename="../src/ui/designer/singleplayer-controller-select-dialog.ui" line="32"/>
        <source>Multiple controllers are connected. Please press any button on the controller you wish to use, or press Esc on the keyboard to use keyboard controls.</source>
        <translatorcomment>Placeholder Google Translate translation</translatorcomment>
        <translation>يوجد أذرعات تحكم متعددة متصلة. يرجى الضغط على أي زر في ذراع التحكم التي ترغب في استخدامها ، أو اضغط على Esc في لوحة المفاتيح لاستخدام ضوابط لوحة المفاتيح.</translation>
    </message>
</context>
<context>
    <name>UpdateDialog</name>
    <message>
        <location filename="../src/updaters/update-dialog.ui" line="14"/>
        <source>Update Available</source>
        <translation>يوجد تحديث</translation>
    </message>
    <message>
        <location filename="../src/updaters/update-dialog.ui" line="26"/>
        <source>A new update is available for Parallel Launcher. Would you like to update now?</source>
        <translation>يوجد تحديث جديد ل Parallel Launcher ، هل ترغب في إلتحديث الآن؟</translation>
    </message>
    <message>
        <location filename="../src/updaters/update-dialog.ui" line="62"/>
        <source>Changelog:</source>
        <translation>سجل التغيير:</translation>
    </message>
    <message>
        <location filename="../src/updaters/update-dialog.ui" line="79"/>
        <source>Don&apos;t remind me again</source>
        <translation>لا تذكرني مرة أخرى</translation>
    </message>
</context>
<context>
    <name>WindowsRetroArchUpdater</name>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="111"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="33"/>
        <source>Checking for RetroArch Updates</source>
        <translation>نبحث عن تحديثات ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="210"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="120"/>
        <source>Download Failed</source>
        <translation>فشل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="211"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="121"/>
        <source>Failed to download RetroArch.</source>
        <translation>فشل تنزيل ريتروارش.</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="218"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="128"/>
        <source>Installing RetroArch</source>
        <translation>يجري تثبيت ريتروارش</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="234"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="144"/>
        <source>Installation Error</source>
        <translation>خطأ في التثبيت</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="235"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="145"/>
        <source>An error occurred attempting to uncompress the portable RetroArch bundle</source>
        <translation>حدث خطأ أثناء محاولة فك ضغط حزمة ريتروارش المحمولة</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="276"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="174"/>
        <source>Install Update?</source>
        <translation>هل تريد تثبيت التحديث؟</translation>
    </message>
    <message>
        <location filename="../src/updaters/retroarch-updater.cpp" line="277"/>
        <location filename="../src/updaters/retroarch-updater.mac.cpp" line="175"/>
        <source>An update for RetroArch is available. Would you like to install it now?</source>
        <translation>تحديث لريتروارش متوفرا. هل ترغب في تثبيته الان؟</translation>
    </message>
</context>
<context>
    <name>WindowsUpdater</name>
    <message>
        <location filename="../src/updaters/self-updater.cpp" line="45"/>
        <source>Unexpected Error</source>
        <translation>خطأ غير متوقع</translation>
    </message>
    <message>
        <location filename="../src/updaters/self-updater.cpp" line="46"/>
        <source>Failed to launch installer.</source>
        <translation>فشل تشغيل Parallel Launcher.</translation>
    </message>
    <message>
        <location filename="../src/updaters/self-updater.cpp" line="57"/>
        <source>Download Failed</source>
        <translation>فشل التنزيل</translation>
    </message>
    <message>
        <location filename="../src/updaters/self-updater.cpp" line="58"/>
        <source>Failed to download the latest installer. Try again later.</source>
        <translation>فشل تنزيل أحدث برنامج تثبيت. حاول مرة أخرى في وقت لاحق.</translation>
    </message>
</context>
</TS>
