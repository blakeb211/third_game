" NOTE - must have the dlls in the same folder 

" Compile SFML Project WITHOUT Audio
term clang++ -O0 -std=c++17 -fuse-ld=lld -Lc:\SFML\lib -IC:\SFML\include -Ic:\Boost172 -lsfml-graphics -lsfml-window -lsfml-system global.cpp entity.cpp builder.cpp leveledit.cpp -o ..\build\level_editor.exe

" Compile SFML Project WITH Audio
"
" term clang++ -std=c++17 -Lc:\SFML\lib -IC:\SFML\include -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system  % -o %.exe




