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

  class CborEncoder
    def add(value)
      case value
      when Integer
        if value < 0
          add_neg_int(-1 - value)
        else
          add_pos_int(value)
        end
      when Numeric then add_auto_float(value)
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
