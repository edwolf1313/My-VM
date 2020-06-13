#!/bin/bash

out=/tmp/std.out

/bin/date '+%a, %e %b %Y %H:%M:%S %Z' > $out

# /usr/bin/curl --http2-prior-knowledge https://www.bankmandiri.co.id/kurs 2>/dev/null | /bin/sed -n 's/.*<tr>.*USD[^0-9]*\([0-9,.]\+\)[^0-9]*\([0-9,.]\+\).*<\/tr>.*/Mandiri\tBeli:\1 Jual:\2/p' >> $out

/usr/bin/curl https://www.bni.co.id/id-id/beranda/informasivalas 2>/dev/null | /bin/sed -n '/<tbody>/p' | /bin/sed -n '1{s/.*USD[^0-9]*\([0-9,.]\+\)[^0-9]*\([0-9,.]\+\).*/BNI\tBeli:\2 Jual:\1/p}' >> $out

/usr/bin/curl https://www.bca.co.id 2>/dev/null | /bin/sed -n '/tbody/,/\/tbody/{/USD/{n;h;n;H;g;s/^[^0-9]*\([0-9,.]\+\)[^0-9]*\([0-9,.]\+\).*/BCA\tBeli:\1 Jual:\2/p}}' >> $out

echo '==========' >> $out

