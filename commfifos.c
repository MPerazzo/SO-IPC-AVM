#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "types.h"
#include "comm.h"

struct Connection {

    int incoming_fd;
    int outgoing_fd;

};

struct Listener {

    char address[32];

};

typedef struct {

    int requester_pid;

} ConnectionRequest;

Connection * comm_connect(char * address) {

    int listener_fd;
    char client_incoming[32];
    char client_outgoing[32];
    ConnectionRequest connectionRequest;
    Connection * connection;

    connection = malloc(sizeof(Connection));

    connectionRequest.requester_pid = getpid();

    sprintf(client_incoming, "/tmp/client_%d_incoming", connectionRequest.requester_pid);
    sprintf(client_outgoing, "/tmp/client_%d_outgoing", connectionRequest.requester_pid);

    mknod(client_incoming, S_IFIFO | 0666, 0);
    mknod(client_outgoing, S_IFIFO | 0666, 0);

    listener_fd = open(address, O_WRONLY);

    write(listener_fd, &connectionRequest, sizeof(ConnectionRequest));

    close(listener_fd);

    connection->incoming_fd = open(client_incoming, O_RDONLY);
    connection->outgoing_fd = open(client_outgoing, O_WRONLY);

    return connection;

}

Listener * comm_listen(char * address) {

    Listener * listener;

    listener = malloc(sizeof(Listener));

    unlink(address);

    mknod(address, S_IFIFO | 0666, 0);

    strcpy(listener->address, address);

    return listener;
}

Connection * comm_accept(Listener * listener) {

    int listener_fd;
    char client_incoming[32];
    char client_outgoing[32];
    ConnectionRequest connectionRequest;
    Connection * connection;

    connection = malloc(sizeof(Connection));

    listener_fd = open(listener->address, O_RDONLY);

    read(listener_fd, &connectionRequest, sizeof(ConnectionRequest));

    close(listener->address);

    sprintf(client_incoming, "/tmp/client_%d_incoming", connectionRequest.requester_pid);
    sprintf(client_outgoing, "/tmp/client_%d_outgoing", connectionRequest.requester_pid);

    connection->outgoing_fd = open(client_incoming, O_WRONLY);
    connection->incoming_fd = open(client_outgoing, O_RDONLY);

    return connection;

}

int comm_write(Connection * connection, char * dataToWrite, int size) {

    return write(connection->outgoing_fd, dataToWrite, size);

}

int comm_read(Connection * connection, char * dataToRead, int size) {

    return read(connection->incoming_fd, dataToRead, size);

}

int comm_disconnect(Connection * connection) {

    close(connection->incoming_fd);
    close(connection->outgoing_fd);

    return 0;

}

/*




                  .,-:-,,.
                .AMMMHMMHHMII,
              .AMMHHMMMHHMMHMMMHHA.
            .AMHHMMMMHIMMMMMMHMHHHHHL
           .AHHIHHMMHIMMMMMMMHMHHHMHLH:
          ,LHHIIHHHHIHMHMMMMMHMHHHLHHAIA
         ;IHHIHHMMH:IHMHMMMMMMHMHIHLIHHIA
        .:IHHII:':'..:.:IHMHMMHHHHIHHLIHH.
       ,I:AVI::.'. .'.'.::IAHMHMHHIALVAIHA
      ,A:AVI::.'. .   . '.:IAVMMHAVLVAIVHHA
     .II:AHI:.'. .      . ':IAVHMHMAHVHIHHH;
    ,A:H:HI::.  .      . .'.:IAIHHAMAIAHIHHLA
   .AIH:AHI:'.            .'.:IIAHHMMAHHIHHIH,
  .AHI:AHI:.:..            .'.VIIHMHMMAIHIHIIA
  ;HHI:AHI:',,'   .    .,:AHVVL:IIHIHMMHHHIHIM;
  ;AII:HIPPT:TPHA. .  .::''. . .VIHHMHMAHIHI:HM
 ;IAIV:HI::.,,,,:'. .  .,:III::.:VIHHHMMHHHHIIHB
 AIHI:HII:I:VCLTV;:.  ..VK CVTPA::IHMHHMHHIHIHV
.LHH::HIHI:IA.TL/::.  ..'VPTTI:'':IIHHHIHMIIHH'
;IHHIHIHHI:. ' '..:.  ..  .'. ' .':HIHMHHMMHIH
IHMHIHIHHI:.   . .:'  . .. .     .:IHHMMMMMHIV
IHHMIHIHHI:.   ..:'.  .. ':. .  ..:HIHMMMMMHV'
HHMHIHIHHI::. ..::L..:-;. ':.. . .:IHMMHMMHIP
HHMHIIHIHHI:.:. .. :''.   .':.. ..:IHMMMHMHV
HIHMMHHHIHI::. ..,.,, ,,,,,.,:.. .:IHMMHMMM'
IIHHIMHMHIHI::. 'LP:'''''''P/. . .:IHMMMHMV
:IIHHMMHMHHII:.. V:-,,,,-''. .  ..:IMHMMMM'
 'IHHMMHHMHHHA:.''::;;;::/' . . .::IMHMMMM
 ;IHMMMHHHMHMHA:...'''''     . ..::':VHHMH:
 :IHHMMHMHHMMMHA:.. .      . ..::'.'.':VHV
 ;IHIHMHHIHMMHMMMA.. . . ...:::'.'.  .. T,
 :IIHMMHIHIHMMMMMMAAL::.::.::':. .   .    ,:.
 ;HIHHMHHIHHMHMMMMMHMHHIHI::.:.. : .   .   AHAA.
.AHIHMMMHMHMHMMHMMMMHMHII::.:.. . .  .  ..:HMMH:
AHV':HHMHHMHMMMHMMMMMHHI::.:.. ..      . .AOAVA/'-..
:I''VHHMMMHHMMMHMMMMHMHI::... :. .   . ..AOOLV/.:.:.:''--...,.
'  .AHIHHMHHMMMMHMMMMMMMI:.... :.    . .AAVAV/.:. . .:  .    .''''-.
  ,:HIIHHMMHMMMMHMMMMHMHI: .:.:: .  . .:ANOO/.:   .: . .  . ..   .. ',,
 ,;P':IHIHHMMHMMMMMMMMHMI:.  .:.. . ..AVAOV/'   .. .:.. .. .       . HA.
,AH::AHIHHHMHMMMMMMMMMMHMA:. .::. ..:.OAVO/. '...::.::.. .  .        HHA
  ' :IIHIHHMHMMMMMMMMMHMHH:..:::.:..AOAVO/:.'..::.:..::.. .       .  HHH
  .::IHHIHMMHMMMMMMMMHMMHII.:.::'.:AOOAV/..: .:.:..:. . .  .   . . . HHH
 ,IHIHHIHMHMMMMMMMHMMHMV:.::::'. .:AOOVV/:.: .:. ..:.. .  .  ..  . . HHH
.:IHHIHHHMHMMMMMMMMMHMV::....:. .:/AOAV/. .:.:.. .. .   . .  . .. . IHHH
 :IHHHMHMMMHMMMMMMHMHHI::...:...:/AOAV/..: :. :.  ..   .    .  .. . AHHH
AIHMHHMMHMMMMMMMMHMHHII:.. .:. ./OAOV/:..:... .   .    .   .  .. .. HHHH
HIHIHHMMHHMHMMHMHHHIHI::.. . ./AO/A//...:.. .   .    .      . .. .  HHHH
HHV'AHHHMHMHHHHIOIHIIII::..../AO/AV/ ..:.:..   .      .    . .. . ..::VH
V'.:HHHMMHMMHHIHOIHHIII:::../VOAVV/.'...:. . .   .       .  . ..::.::III
 .:HHHHMMHMHHMHOOIIII:I:::./OAHVO/: ..:.. . .  .   .       . .. .::::III
.:HHIHHMHMMMHHIOOIHII:::..OO//AO/.. .::.. .  . .     .    . .. .::::IIIH
HIHIIHHMMHMMMHHIOOIII:::.OOO/AO//..:..:..  . ..  .  .     .. ..I:::IIIHH
IHHIIHHMHMMHHIHI:OOO;:.OO/A:AO//. ...:.. ..  .  .          . .:IAI::IIIH
HHIHHHVVHMMHHIHI:::OOOO:O/.AO//. .. .... . ..  .          . ..:IIAI::IIH
VHHHV'.:HMHHIHHI::.:..A/A:AVO/:. . ... ..  . .   .      . ..:.:IIHAHIIII
 VHV   VHHAIVHI:.'...A/A:AOOV/... . .. .  .   .        . ..:.::IIHMHAIIH
       'VHIHA'.'O:O:W:.:A/AO/.. .. .  .   .  .         .. .:..::IHMMHIII
       :V:I:I:.:... ..:A:IOO:.:.. . .  . ....          . ..:.:::IHHHHHII
       O:I:I::. .. . /A/:IOO::. .. .  ..:::::.        . ..:.::::IHMHHVII
      O.I:.. .  . .:AV.:.IO:::.. .    ..:II::..       .. .:.::::IHMHHIII
     O :I:. .  .  /A:...IOO::.. .. . . ..::.:.       . ...:.::::IHMHVIII
     O.:I.      .AW'. ..IOI::.:.. .. .  .:.:.        . .:..:::::IMHHIIII
    ,::.I.  ..AHW'  ...IOII:I:.:.. .. .. .  .  .    . ..:..:.:::IMMVIIII
   B:I:.:I.AHW'     . :IOII:I::.:... ... .. ..  . . ....::.:.:::IMMHIIII
   'VHHHHV:' .     . :V.OO:I::.:... .:.. ... .. .. ..:::. .::.::AHMVIIII
     V:. .V.    . . :V:HOO:II::.::.:. :.... ..:...::I::. ...::::MHMIIIII
     ':.. .V.. . .:V:.A:OO:III::::::.::..:.:.:I.::::.. . .. .:::MMHIIIII
      ':....V:OOO;V..:W:OO::IIII:II::II:I:I:II:::::.    .. ..:::MHHAIIII
       '::.:..'.'. ..W:.OO.:II:::II::IIII:II:::.:..    .  ...:::MMHMIIII
        ':I:.:.:....W:..OO:::II:::::I::::::..:..    .'.    ..:::MMHVIIII
         ':III::.:.W/::OO:::.:I::I:::::.:.:.. .. .. ..    ...:::MMHIIIII
          OO'WII:::W:I:OO::..:I:::II::..:... .. ... .     ..:::AMMMIIIHH
          'O O'WI:W:II:OO.:..:.:II:::I:..:... ... .      . ..::MHVIIIHHH
           OO'OOOV::II:OO:.. .::.:II::.:.. .. .. .    . .:..::AMMIIIIHHH
            O   II:.:::OO.:.. ...:.:II:.:.. ... . . .  ...:.::HMMIIIIHHH
            OO  A:::.:OO:... . ...:.:I.::.:... . . .  . ..:::AMHMIIIIHHH
            OO AM::.::OO::... . ...:.:.:.:.:..:.. . .. ...::IMMMMIIIIHHH
             O HHM.:.:OO::.. . . .:.:.:.:.:..:... .. .  ..:AMHMMMIHIIHHI
             OOMM..:.:OO.:... . ...:...:..:.:..:.. .. . ..:HHMMHMIHIIHHI
            AMOOV..:.:OO:.:... . . ..:..:..:.::..:.. .. .:AHHMMMMIHIIHHH
           AMMOO.:.:::OO::.:.:.. . .:..:....::..:.::..:.:IHHMMMHVIHHIHHH
          AMMMOO.::.:OO::..:..:.. ...:..:..:..:..:.::.:::IHHMMMHAIHHIHHH
         AMMMM'OO::::OO:::..:... .. ...:..:..:..:.:.:::.:IHHMMMHIHHHIHHH
         MMMV..:OO::OO:::.::.::.... .:...:...:.:..::.::::IHHMMMHIHHHIHHH
        AMM'... .:OOO::::.:::..:..:. ..:. .:..:..:.::::.:::HHMMMIHHIIHHH
       AMM' . .. .:::::.:::.::...:.:. . .. . ..::.::.:::::IIVMMVIHHIIHHH
       MM' .  . .:.:.::I:.:.:.:::..::.. . .. ..::..::.::I:IIIVMIIHHIIHHH
      AM' .. . . .::.:.::I:.::.:.:::..:. . . . ..:..::.:I:IIIIVIHHHIIHHH
     AM' . ... . ..:::.::..::.:::..:..:.. .  .. .:.:.:I:IIIHIIVIHHHIIHHH
    AMV .. .. .  .. .::.::..:.::::.:.:. .. . ... ..:I:IIIHIH:IIHHHHIIHII
   AMV . .. ...  . .:. :.:...:I:..:.:.:.. . .:. ..:.:.:IIHIHH:AHHHHIIHII
  AMM' .. .. ...  ..:::... . NV::..:..:.. .... ..:.:::IIHIHHI:HHHHIIIIII
 AMMV . ... .:.. ..: ..:. . . .  .:..:. . .. .:..::::IIIHIHHI:VHHHHIIIIH
AMMM' .. .:. .... .::..:.. . .. . ..  .. . ..:...:::I:IHIHHHH:I:VHHHHIII
MMMV . ... ... ...::. ...:.. ... . ...:..:..:.:.::I:I:IHIHHVII::HHHHHHIH
MMM'. ... .:..:..:...:... ..:. .. ...:.....::.:.:::I:IHHIHHIII:AHHHHHI'H
MMV .. ....:.::.:..:... ... ..:.... .. ..:.. .:...I:IIHIHHVIII.HHHHHH' H
MMI ... ..::..::...::... . .:..:...:... ...:..:..:::IIIHIHIIIV.HHHHHH AH
MM'.. ... ..::.:::..::...:.:.:. .:...:..:..:.:..::::I:IIHIIII..HHHHHH HH
MM.. .....:.::..::.:.:.::..::.::..::... ...:.::.:::I:IIHIHIII..HHHHHVAHH
MV...: ..:.:::.::.:.:::..:.:.:::.:.:.:.:..:..:::.:::IIHIHIHHV.AHIHHH'AHH
MI.. ..:..::::I:.:.::.:::.:::.:.:::.:.:.:.:.:I:.:::I:IIHIHVHI.HHHHHH.HHH
MI.. . ..:..:::I::.::..:.:.:.:.:.::.::..:.:II::.::::I:IHIIVH:.HHHHHH.HIH
MI... . ..:..:::II::I::.:.I.:.:.:.::.::II:I:.:.:.::I:IIIIVHV:AHHHHHHA IH
MA.:..:. ..:. .::IIHHHI:I.IHI.::I:I:I:I::.::. ..:..::I:IIHHIIHHHHHHHH IH
MM..:...... . .::IHHHIHIIHHIHIIH:IHII:::. .:.. ..:.:::IIAHHIHHHHHHHHV HH
MM.:. .:... .:..:IHHMHWHMHMWMHMHMIIH::..:.. ..:..:.::IIAHHHIHHIHHHHH' IH
MM:..:.:. .. ...::IMMHWMMHMWMMHMHHMI:.:. ... . .::.:IIHAHHHIHIIH HHH  IH
MM::.:. ... .. ..:IMWMMWMWMWMMMWHII.:... ... . ..:.:.:IHIHHIHHIV HHH  IH
MMI::.:. ... .. .::VMMMMWMWMWMMHV:.:.. . . .. ..:.:..:IHIIHHHMM AHHH. IH
MMA:::.:.:. . . ..::VMMMMMWMMMHV:.:... . .  ....:..:.:IHHIHHHHMMMHIHI IH
MMM::::.:.:. . ....::VMMMMMMMV:::.:. . ..  . ... .:..:IHIIHHHHMMMHIHI IH
MMMI:::.:. . ... ...::VMMMWMV::..:... . . . .. .::..:IIHIIHHHHHHHIHHI.'H
MMHM:::..:.. .. ...:.::IWMMV:::.:.. .. . . . ...:.:.:IIHIIHHMHHHHIHHI
MHMMI:::.:. . ... ...:::II:::.:... .. .. . . . :.:.:IIHIIHHHMMHHHIIHII
HIHMA::::.:. .. ... ..::II::.:.:.. . . . .. . :.:.:.IIHIVVIHIIHHHHHHII
IVVMM::::.:.:. . ... .::II:.::..:.. .. .. . . .:.:::IHH:.:HHIIHIHH':-'
'::VMI:::.:.:.. .. . .::II.::.:.:. .. .. . . ..:..::IHH..:HIIIHIHH
    HA::::.:..  ... ...:II.:.:.:... . .. .. ..:..:.:IHII..HHIIHIHI
    HM:::::.:. . .. ...:II.::.:.::.. ...  .  ..:.:::IHIH..HIIIHIH'
   IHMA:::.:... .. .. .:II:.:.::.:.. .. .. .. .:.:::HHIH..VIIIHIH
   HHHM::::.:.. .. ....::II:.:..:.:.. . . . . ..::::HIHH...HIIHHH
   HHMMA::::.:. . .. ..::II.:.:.:.:... . . .. .:.:::HIHHA..HHHHHV
   ''MMM:::::.:. . .. .::II:.:.::.:.. .. .. . .:::::HIHHH..HHHHVI
     VMMI::::.:. ... ..:::II:.:.::.:.. .. . . .:.:::H:HHI...IHHII
      MHA:::.:.. .. ....::II.:.:.::. . ... . . .::::I:HI I..IHHHV
      MMM::::.:. . .. ..::IA:.:.:.:.. .. .. .. ..:::I.H:.I..IHVH
      V'HA:::..:. .. .. ::IIH:.:.:.:.. .. ..  ..:.:I:.II.I.:VV'V
          :::..:... .. .::IIH..:.::.:.. . ... . .:.::.II:I::'III
          ':::.:.. ... .::IIH::.:.:.:. .. .. . .:..::.::.:::  HI
           ::::.:.. .. .::IIH:.:.::.:. . .. . . .:...:.:.:.'  HI
           '::::.. .. ..::IIH::.::.:.:.. ... . . .:.:.:.:.:   II
*/
