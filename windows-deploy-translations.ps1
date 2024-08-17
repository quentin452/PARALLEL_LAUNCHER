mkdir templang
cp lang/*.ts templang
cd templang

mv parallel-launcher_de.ts app.de.ts
mv parallel-launcher_es.ts app.es.ts
mv parallel-launcher_fr.ts app.fr.ts
mv parallel-launcher_fr_ca.ts app.fr_ca.ts
mv parallel-launcher_it.ts app.it.ts
mv parallel-launcher_ar.ts app.ar.ts
mv parallel-launcher_pt.ts app.pt.ts

curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_de.ts -UseBasicParsing -OutFile qt.de.ts
curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_es.ts -UseBasicParsing -OutFile qt.es.ts
curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_fr.ts -UseBasicParsing -OutFile qt.fr.ts
curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_it.ts -UseBasicParsing -OutFile qt.it.ts
curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_ar.ts -UseBasicParsing -OutFile qt.ar.ts
curl https://raw.githubusercontent.com/qt/qttranslations/dev/translations/qtbase_pt_BR.ts -UseBasicParsing -OutFile qt.pt.ts

C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.de.ts qt.de.ts -o parallel-launcher_de.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.es.ts qt.es.ts -o parallel-launcher_es.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.fr.ts qt.fr.ts -o parallel-launcher_fr.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.fr_ca.ts qt.fr.ts -o parallel-launcher_fr_ca.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.it.ts qt.it.ts -o parallel-launcher_it.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.ar.ts qt.ar.ts -o parallel-launcher_ar.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lconvert.exe -i app.pt.ts qt.pt.ts -o parallel-launcher_pt.ts

C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_de.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_en_us.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_en_gb.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_es.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_fr.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_fr_ca.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_it.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_ar.ts
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\lrelease.exe parallel-launcher_pt.ts

rm *.ts
rm ..\build\release\translations\parallel-launcher_*.qm -ErrorAction SilentlyContinue
cp *.qm ..\build\release\translations
cp *.qm ..\build\release\translations
rm *.qm
cd ..
rm templang
