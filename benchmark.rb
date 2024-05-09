require 'benchmark'
require 'securerandom'
require 'cbor'
require 'json'


n = 1000
x1 = 1000.times.map { [SecureRandom.hex(10), SecureRandom.rand(1000)] }.to_h

# x2 = 2000.times.map { [SecureRandom.hex(10), SecureRandom.hex(50)] }.to_h

# x3 = TestData.test_hash(1000)
x = x1
Benchmark.bm(18) do |bm|
  bm.report(:ext_encode) { n.times do  CBOR.encode(x) ;  end }
  bm.report(:ruby_encode) { n.times do Aws::Cbor::CborEngine.encode(x); end }
  bm.report(:crt_encode) { n.times do Awscrt.cbor_encode(x); end }
  bm.report(:crt_auto_encode) { n.times do Awscrt.cbor_crt_auto_encode(x); end }
  bm.report(:json_dump) { n.times do   ; JSON.dump(x); end }
end


# x1 = data_size.times.map { [SecureRandom.hex(10), SecureRandom.rand(1000)] }.to_h
#                          user     system      total        real
# ext_encode           0.032612   0.001302   0.033914 (  0.034210)
# ruby_encode          1.829052   0.008595   1.837647 (  1.843467)
# crt_encode           0.322202   0.002333   0.324535 (  0.325208)
# crt_auto_encode      0.081432   0.009876   0.091308 (  0.091480)
# json_dump            0.101465   0.007174   0.108639 (  0.109086)

# x2 = 2000.times.map { [SecureRandom.hex(10), SecureRandom.hex(50)] }.to_h
#                          user     system      total        real
# ext_encode           0.137817   0.072950   0.210767 (  0.211622)
# ruby_encode          4.046183   0.026118   4.072301 (  4.184712)
# crt_encode           0.931974   0.135230   1.067204 (  1.070094)
# crt_auto_encode      0.283512   0.139248   0.422760 (  0.423502)
# json_dump            0.483778   0.076138   0.559916 (  0.561766)


# x3 = TestData.test_hash(1000)
#                          user     system      total        real
# ext_encode           0.227053   0.107459   0.334512 (  0.336187)
# ruby_encode          4.607834   0.146202   4.754036 (  4.806710)
# crt_encode           1.074157   0.032521   1.106678 (  1.109796)
# crt_auto_encode      0.347386   0.030279   0.377665 (  0.379329)
# json_dump            0.669862   0.031953   0.701815 (  0.703731)