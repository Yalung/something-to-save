#! /bin/bash
find . -maxdepth 1 -name '[.][a-zA-Z0-9]*' -exec cp -a {} code/mygithub/something-to-save/myconf/ \;
cd code/mygithub/something-to-save
git add -A
git commit -a -m "update"
git push


