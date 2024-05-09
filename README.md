# Awscrt

TODO: Delete this and the text below, and describe your gem

Welcome to your new gem! In this directory, you'll find the files you need to be able to package up your Ruby library into a gem. Put your Ruby code in the file `lib/awscrt`. To experiment with that code, run `bin/console` for an interactive prompt.

## Installation

TODO: Replace `UPDATE_WITH_YOUR_GEM_NAME_IMMEDIATELY_AFTER_RELEASE_TO_RUBYGEMS_ORG` with your gem name right after releasing it to RubyGems.org. Please do not do it earlier due to security reasons. Alternatively, replace this section with instructions to install your gem from git if you don't plan to release to RubyGems.org.

Install the gem and add to the application's Gemfile by executing:

    $ bundle add UPDATE_WITH_YOUR_GEM_NAME_IMMEDIATELY_AFTER_RELEASE_TO_RUBYGEMS_ORG

If bundler is not being used to manage dependencies, install the gem by executing:

    $ gem install UPDATE_WITH_YOUR_GEM_NAME_IMMEDIATELY_AFTER_RELEASE_TO_RUBYGEMS_ORG

## Usage

### Building CRT

TODO:  This obviously needs a nice cmakelist.txt that builds everything together and supports install. But for now, this works.


```
cd crt/aws-c-common-build
cmake ../aws-c-common -DCMAKE_INSTALL_PREFIX=../install
make -j 12
make install
```

Building:
```
rake build
```

Testing:
```
bin/console
```

```ruby
encoder = Awscrt::CborEncoder.new
encoder.add_string("test")
d = encoder.encoded_data
Aws::Cbor::CborEngine.decode(d)
```


```ruby
encoder = Awscrt::CborEncoder.new
encoder.start_array(3)
encoder.add_string("test")
encoder.add_string("value2")
encoder.add_float(1.123)
d = encoder.encoded_data
Aws::Cbor::CborEngine.decode(d)


encoder = Awscrt::CborEncoder.new
encoder.start_map(2)
encoder.add_string("k1")
encoder.add_pos_int(1)
encoder.add_string("k2")
encoder.add_neg_int(1)
d = encoder.encoded_data
Aws::Cbor::CborEngine.decode(d)

d = Awscrt.cbor_encode({a: 1, b: -10})
Aws::Cbor::CborEngine.decode(d)

encoder = Awscrt::CborEncoder.new
encoder.crt_auto_add({a: 1, b: 2})
d = encoder.encoded_data
Aws::Cbor::CborEngine.decode(d)

```

Maybe useful for memory debugging: https://github.com/Shopify/ruby_memcheck

## Resources
* https://docs.ruby-lang.org/en/master/extension_rdoc.html - general extesion reference
* https://silverhammermba.github.io/emberb/c/ - a good overview of the ruby c API
* https://github.com/ruby/ruby/blob/master/include/ruby/ruby.h  - all of the VM and metaprogramming functions
* https://github.com/ruby/ruby/blob/master/include/ruby/intern.h - all of the functions for interacting with Ruby’s built in classes.
* https://github.com/cabo/cbor-ruby/blob/master/ext/cbor/packer.c#L132 - cbor gem's auto add function

## Development

After checking out the repo, run `bin/setup` to install dependencies. Then, run `rake spec` to run the tests. You can also run `bin/console` for an interactive prompt that will allow you to experiment.

To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org).

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/[USERNAME]/awscrt.

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).
