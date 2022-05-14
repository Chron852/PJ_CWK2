if [ -d /usr/local/include/ui.h ]; then
  echo "installed libui"
  LIBUI_FLAG=1
fi

if [ $LIBUI_FLAG = 0 ]; then
    echo "Install libui"
    git clone https://github.com/andlabs/libui.git
    cd libui || exit
    meson setup build
    ninja -C build
    cd build/meson-out || exit
    ln -sf libui.so.0 libui.so
    cd ../.. || exit
    install build/meson-out/libui.so /usr/lib/
    install build/meson-out/libui.so.0 /usr/lib/
    install ui.h /usr/include/
    install ui_unix.h /usr/include/
    cd .. || exit
    #rm -rf libui
fi
