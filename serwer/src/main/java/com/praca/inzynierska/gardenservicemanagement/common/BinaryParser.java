package com.praca.inzynierska.gardenservicemanagement.common;

public class BinaryParser {

    public static String getIpAddressFromInt32(long value) {
        var octets = new long[4];
        for(int i=0; i<4;i++) {
          octets[i] = (value >> (i * 8)) & 0xFF;
        }
        return String.format("%d.%d.%d.%d", octets[3],octets[2],octets[1],octets[0]);
    }

    public static int convertIpAddressToInt32(String value) {
        var octets = value.split("\\.");
        int ipConvertedToInt32 = 0;
        for(int i=0;i<4;i++) {
            ipConvertedToInt32 += Integer.parseInt(octets[i]) << (24 - i * 8);
        }
        return ipConvertedToInt32;
    }

    public static String getMacAddressFromInt64(long value) {
        var octets = new long[8];
        for(int i=0; i<8;i++) {
            octets[i] = (value >> (i * 8)) & 0xFF;
        }
        return String.format("%s:%s:%s:%s:%s:%s", Long.toHexString(octets[7]).toUpperCase(),
                                                  Long.toHexString(octets[6]).toUpperCase(),
                                                  Long.toHexString(octets[5]).toUpperCase(),
                                                  Long.toHexString(octets[4]).toUpperCase(),
                                                  Long.toHexString(octets[3]).toUpperCase(),
                                                  Long.toHexString(octets[2]).toUpperCase());
    }

    public static long convertMacAddressToInt64(String value) {
        var octets = value.split(":");
        long macConvertedToInt64 = 0;
        for(int i=0;i<6;i++) {
            macConvertedToInt64 += Long.parseLong(octets[i],16) << (56 - i * 8);
        }
        return macConvertedToInt64;
    }

}
