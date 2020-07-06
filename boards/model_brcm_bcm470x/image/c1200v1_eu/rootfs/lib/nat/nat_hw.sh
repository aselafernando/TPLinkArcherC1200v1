# fastpath setting for brcm

# $1=status, $2=init/reset
nat_hw_enable() {
    # echo nat_hw_enable $@ > /dev/console
    # we do not set during init
    if [ $2 != "reset" ];then
        return
    fi

    if [ $1 = "1" ];then
        # flow_fwd  fastpath
        nvram set ctf_disable=0
    else
        # flow_fwd  host
        nvram set ctf_disable=1
    fi

    nvram commit

    echo 'reboot by nat boost.' > /dev/console
    reboot
}
