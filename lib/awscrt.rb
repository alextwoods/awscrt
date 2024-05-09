# frozen_string_literal: true

require_relative "awscrt/version"
require_relative "awscrt/awscrt"
require_relative 'awscrt/cbor/cbor_engine'

module Awscrt
  class Error < StandardError; end
  # Your code goes here...

  def self.cbor_encode(value)
    encoder = Awscrt::CborEncoder.new
    encoder.add(value)
    encoder.encoded_data
  end

  def self.cbor_crt_auto_encode(value)
    encoder = Awscrt::CborEncoder.new
    encoder.crt_auto_add(value)
    encoder.encoded_data
  end

  class CborEncoder
    def add(value)
      case value
      when Integer
        if value < 0
          add_neg_int(-1 - value)
        else
          add_pos_int(value)
        end
      when Numeric then add_float(value)
      when Symbol then add_string(value.to_s)
      when String then add_string(value)
      when Array
        start_array(value.size)
        value.each { |di| add(di) }
      when Hash
        start_map(value.size)
        value.each do |k, v|
          add(k)
          add(v)
        end
      else
        raise UnknownTypeError, value
      end
      self
    end
  end
end

module TestData
  # generate predictable, but variable test values of different types
  def self.random_value(i=0, seed=0)
    r = Random.new(i+seed) # use the index as the seed for predictable results
    case random_number(r, 5)
    when 0 then "Some String value #{i}: #{'a' * random_number(r, 5000)}"
    when 1 then r.rand # a float value
    when 2 then random_number(r, 100000) # a large integer
    when 3 then (0..random_number(r, 100)).to_a # an array
    when 4 then {a: 1, b: 2, c: 3} # a hash
    else
      "generic string"
    end
  end

  # generate a predictable, but variable hash with a range of data types
  def self.test_hash(n_keys=5, seed=0)
    n_keys.times.map { |i| ["key#{i}", random_value(i, seed)]}.to_h
  end

  def self.random_number(r, n)
    (r.rand*n).floor
  end

end
