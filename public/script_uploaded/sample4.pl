#!/usr/bin/env perl

my $out = '/tmp/std.out';

my $curl_out = `/usr/bin/curl https://odb.org/feed/ 2>/dev/null`;

open(my $fh, '>', $out);
foreach(split /\n/, $curl_out) {
  if (/guid/) {
    my ($pre, $url, $post) = split />/;
    ($url, $post) = split '<', $url;
    print $fh "$url\n";
  }
}
close($fh);
