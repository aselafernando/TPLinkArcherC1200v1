# tpsdk专有功能 by huluyao 20140423

fw_load_tpcmd() {    
    # for guest_network
    local rule="-p tcp -m multiport --ports 80,22,20002"
    fw add 4 f block_entry_list
    fw add 4 f zone_lan block_entry_list 1 { "$rule" }
    fw add 4 f ftp_access
    fw add 4 f zone_wan ftp_access 1

    # for local management
    fw add 4 f local_mgnt
    fw add 4 f local_mgnt DROP

    # for remote management
    # do nothing
}

