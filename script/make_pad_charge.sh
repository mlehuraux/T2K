cp ~/suvorov/macros/pad_charge.C ~/INSTALLFCC/macros
cd ~/INSTALLFCC/macros
make SRC="mydict.C pad_charge.C"  EXE=pad_charge.EXE
rm pad_charge.C
mv pad_charge.EXE ~/suvorov/script
cd ~/suvorov/