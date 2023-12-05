package com.praca.inzynierska.gardenservicemanagement.application;

import com.praca.inzynierska.gardenservicemanagement.common.BinaryParser;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestInstance;
import org.springframework.boot.test.context.SpringBootTest;


@SpringBootTest
@TestInstance(TestInstance.Lifecycle.PER_CLASS)
public class BinaryParserTest {


    @Test
    void shouldConvertFromInt32ToIp() {
        Assertions.assertEquals(BinaryParser.getIpAddressFromInt32(285255872L), "17.0.168.192");
        Assertions.assertEquals(BinaryParser.getIpAddressFromInt32(385255872L), "22.246.137.192");
        Assertions.assertEquals(BinaryParser.getIpAddressFromInt32(185255872L), "11.10.199.192");
        Assertions.assertEquals(BinaryParser.getIpAddressFromInt32(-285255872L), "238.255.87.64");
    }

    @Test
    void shouldConvertFromInt64ToMac() {
        Assertions.assertEquals(BinaryParser.getMacAddressFromInt64(3477672765554641344L), "30:43:2C:F4:6B:EF");
        Assertions.assertEquals(BinaryParser.getMacAddressFromInt64( 1477672763354641322L), "14:81:BF:8C:9A:5");
        Assertions.assertEquals(BinaryParser.getMacAddressFromInt64( -7477272763314641320L), "98:3B:64:9:BD:ED");
        Assertions.assertEquals(BinaryParser.getMacAddressFromInt64( 0L), "0:0:0:0:0:0");
    }

//    private Stream<Arguments> provideValuesForIpFromInt32Converter() {
//        return Stream.of(
//                Arguments.of(285255872L, "true"),
//                Arguments.of(385255872L, "true"),
//                Arguments.of(185255872L, "true"),
//                Arguments.of(285255872L, "true")
//        );
//    }

}
