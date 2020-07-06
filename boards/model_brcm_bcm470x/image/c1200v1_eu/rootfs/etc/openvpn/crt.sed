:begin
/Certificate:/,/-----BEGIN CERTIFICATE/ {
	/-----BEGIN CERTIFICATE/! {
		$! {
			N;
			b begin
		}
	}
	s/Certificate.*-----BEGIN CERTIFICATE/-----BEGIN CERTIFICATE/;
}
